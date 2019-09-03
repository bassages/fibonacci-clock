#include <ESP8266WiFi.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal

// See http://fastled.io/
#include <FastLED.h>
// See https://github.com/ropg/ezTime
#include <ezTime.h>
// See https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html
#include <FS.h>
// See https://arduinojson.org/
#include <ArduinoJson.h>

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

// -------------- Config ---------------

#define BUZZER_HOUR_TYPE_OFF 0
#define BUZZER_HOUR_TYPE_COUNTDOWN_TO_HOUR 1
#define BUZZER_HOUR_TYPE_CUCKOO 2

struct Config {  
  byte mode;
  byte brightness; // Range 0-255

  String wifiSsid;
  String wifiPassword;

  byte clockPalette;
  String timezoneLocation;
  
  String lampColorHex;
  byte lampColorR;
  byte lampColorG;
  byte lampColorB;
  
  String stroboColorHex;
  byte stroboColorR;
  byte stroboColorG;
  byte stroboColorB;
  int stroboSpeed;

  String glowColorHex;
  byte glowColorR;
  byte glowColorG;
  byte glowColorB;
  int glowSpeed;

  bool buzzerStartup;
  bool buzzerHalfHour;
  byte buzzerHour;
  bool buzzerSecond;
  int buzzerSecondFrequency;
};

const char* configFileFullPath = "/config.json";
Config config; // <- global configuration values

// -------------- Accesspoint ---------------
IPAddress accesspointIp(192, 168, 1, 1);
DNSServer dnsServer;

// -------------- Webserver ---------------
const char* host = "fibonacci-clock";
// Create an instance of the webserver, specify the port to listen on as an argument
ESP8266WebServer webserver(80);

// WiFi
boolean connectedToNetwork = false;
WiFiEventHandler connectedEventHandler, gotIpEventHandler, disconnectedEventHandler;

// -------------- Clock ---------------
#define NUMBER_OF_PIXELS 5 // Number of "areas", this clock has five: 1, 2, 3, 5, 8
#define NUMBER_OF_CLOCK_COLOR_PALETTES  11
boolean forceRefresh = true;

// Ordering: off, hours, minutes, both hours AND minutes
CRGB clockPalettes[NUMBER_OF_CLOCK_COLOR_PALETTES][4] = {
  { // #1 RGB
    CRGB::Black, CRGB::Red, CRGB::Green, CRGB::Blue
  },
  {  // #2 Mondrian
    CRGB::Black, CRGB(255,10,10), CRGB(248,222,0),CRGB(10,10,255)
  },
  {  // #3 Basbrun
    CRGB::Black, CRGB(80,40,0), CRGB(20,200,20),CRGB(255,100,10)
  },
  {  // #4 80's
    CRGB::Black, CRGB(245,100,201), CRGB(114,247,54), CRGB(113,235,219)
  },
  {  // #5 Pastel
    CRGB::Black, CRGB(255,123,123), CRGB(143,255,112), CRGB(120,120,255)
  },
  {  // #6 Modern
    CRGB::Black, CRGB(212, 49,45), CRGB(145,210,49), CRGB(141,95,224)
  },
  { // #7 Cold
    CRGB::Black, CRGB(209,62,200), CRGB(69,232,224), CRGB(80,70,202)
  },
  { // #8 Warm
    CRGB::White, CRGB(237,20,20), CRGB(246,243,54), CRGB(255,126,21)
  },
  { //#9 Earth
    CRGB::White, CRGB(70,35,0), CRGB(70,122,10), CRGB(200,182,0)
  },
  { // #10 Dark
    CRGB::White, CRGB(211,34,34), CRGB(80,151,78), CRGB(16,24,149)
  },
  { // #11 Dutch flag
    CRGB::Black, CRGB::Red, CRGB::White, CRGB::Blue
  }
};

// -------------- LEDs ---------------
#define LED_DATA_PIN    2 // D4 on Wemos D1 mini
#define LED_TYPE        WS2812B
#define LED_COLOR_ORDER GRB
#define NUMBER_OF_LEDS  12

CRGB leds[NUMBER_OF_LEDS];
uint8_t gHue = 0; // rotating "base color" used by some of the patterns
byte pixels[NUMBER_OF_PIXELS];
byte previousBrightness;

byte glow = 4;
boolean glowDown = false;
int glowSpeed = 10;

// -------------- Buzzer I/O ---------------
#define BUZZER_PIN 16 // D0  on Wemos D1 mini

// -------------- Date/time ---------------
Timezone dateAndTime = nullptr;
byte oldHours = 0;
byte oldMinutes = 0;
byte oldSeconds = 0;

void setup() {
  setupSerial();
  Serial.println("Start setup");

  setupFileSystem();
  loadConfiguration(configFileFullPath, config);
  previousBrightness = config.brightness;
  glow = 4;
  glowDown = false;
  
  setupLeds();
  // SPIFFS.remove(configFileFullPath); // Uncomment to remove config file (to test the wifi-setup flow)

  setupBuzzer();
  setupNetwork();

  if (config.buzzerStartup) {
    playStartupTune();  
  }
  
  if (connectedToNetwork) {
    setupDateTimeSync();
  }

  Serial.println("Setup finished");
}

void loop() {
  if (config.mode != 8) {
    if (previousBrightness != config.brightness) {
      FastLED.setBrightness(config.brightness);
    }
  }
  
  if (config.mode == 0) {
    loopClock();
  } else if (config.mode == 1) {
    loopLamp();
   } else if (config.mode == 2) {
    loopLampWithGlitter();
  } else if (config.mode == 3) {
    loopLampStrobo();
  } else if (config.mode == 4) {
    loopPixelStrobo();
  } else if (config.mode == 5) {
    loopLedStrobo();
  } else if (config.mode == 6) {
    loopRainbow();
  } else if (config.mode == 7) {
    loopRainbowWithGlitter();
  } else if (config.mode == 8) {
    loopGlow();    
  } else {
    loopOff();
  }

  dnsServer.processNextRequest();
  webserver.handleClient();
}

void loopClock() {
  if (!connectedToNetwork) {
    return;
  }
  events(); // Process date/time events

  if (secondChanged()) {
    byte currentHour = dateAndTime.hourFormat12();
    byte currentMinute = dateAndTime.minute();
    byte currentSeconds = dateAndTime.second();

    Serial.print(currentHour);
    Serial.print(":");
    Serial.print(currentMinute);
    Serial.print(":");
    Serial.println(currentSeconds);
    
    if ((oldHours != currentHour || oldMinutes/5 != currentMinute/5) || forceRefresh) {
      forceRefresh = false;
      setClockPixels(currentHour, currentMinute);
      FastLED.show();
    }

    clockBuzzer(currentHour, currentMinute, currentSeconds);

    // Update NTP every ten minutes, 
    // when only on times that no beeps should occur and no LEDs need to be set.
    // This is to prevent timing interference between these processes causing 
    // for example buzzer beeps to be "incomplete".
    if ((oldMinutes == 7 && currentMinute == 8) 
        || (oldMinutes == 17 && currentMinute == 18)
        || (oldMinutes == 27 && currentMinute == 28)
        || (oldMinutes == 37 && currentMinute == 38)
        || (oldMinutes == 47 && currentMinute == 48)
        || (oldMinutes == 57 && currentMinute == 58)) {

      Serial.println("Update NTP");
      updateNTP();
    }

    oldHours = currentHour;
    oldMinutes = currentMinute;
    oldSeconds = currentSeconds;
  }
}

void loopLampStrobo() {
  setAllPixels(CRGB(config.stroboColorR, config.stroboColorG, config.stroboColorB));
  FastLED.show();
  delay(config.stroboSpeed);

  setAllPixels(CRGB::Black);
  FastLED.show();
  FastLED.delay(config.stroboSpeed);
}

void loopPixelStrobo() {
  setRandomPixel(CRGB(config.stroboColorR, config.stroboColorG, config.stroboColorB));
  FastLED.show();
  FastLED.delay(config.stroboSpeed);
}

void loopLedStrobo() {
  setRandomLed(CRGB(config.stroboColorR, config.stroboColorG, config.stroboColorB));
  FastLED.show();
  FastLED.delay(config.stroboSpeed);
}

void loopLamp() {
  setAllPixels(CRGB(config.lampColorR, config.lampColorG, config.lampColorB));
  FastLED.show();
  FastLED.delay(10);
}

void loopLampWithGlitter() {
  for (int i=0; i<NUMBER_OF_PIXELS; i++) {
    setPixel(i, CRGB(config.lampColorR, config.lampColorG, config.lampColorB));
  }
  addGlitter(80);

  FastLED.show();  
  FastLED.delay(10); // insert a delay to keep the framerate modest
}

void loopRainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUMBER_OF_LEDS, gHue, 7);

  FastLED.show();
  FastLED.delay(10);
  
  EVERY_N_MILLISECONDS(20) { gHue++; } // slowly cycle the "base color" through the rainbow  
}

void loopGlow() {
  if (glowDown) {
    glow = glow - 1;
    
    if (glow == 4) {
      glowDown = false;
    }
    
  } else {
    glow = glow + 1;

    if (glow == config.brightness) {
      glowDown = true;
    }
  }

  FastLED.setBrightness(glow);
  setAllPixels(CRGB(config.glowColorR, config.glowColorG, config.glowColorB));
  FastLED.show();
  FastLED.delay(config.glowSpeed);
}

void loopRainbowWithGlitter() {
    // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUMBER_OF_LEDS, gHue, 7);
  // Plus some random sparkly glitter
  addGlitter(80);

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/120);
  
  EVERY_N_MILLISECONDS(20) { gHue++; } // slowly cycle the "base color" through the rainbow  
}

void addGlitter(fract8 chanceOfGlitter) {
  if (random8() < chanceOfGlitter) {
    leds[random16(NUMBER_OF_LEDS)] += CRGB::White;
  }
}

void loopOff() {
  setAllPixels(CRGB::Black);
  FastLED.show();
  delay(100);
}

void setupSerial() {
  Serial.begin(115200);
  while (!Serial) continue;
}

void setupLeds() {
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,LED_DATA_PIN,LED_COLOR_ORDER>(leds, NUMBER_OF_LEDS).setCorrection(TypicalLEDStrip);
  // set LED master brightness control
  FastLED.setBrightness(config.brightness);
}

void setupDateTimeSync() {
  // By default, ezTime is set to poll pool.ntp.org about every 30 minutes.
  // These defaults should work for most people, but you can change them by specifying a new server with setServer 
  // or a new interval (in seconds) with setInterval. 
  // If you call setInterval with an interval of 0 seconds or call it as setInterval(), no more NTP queries will be made.
  setInterval(); // We'll sync manually, see main loop
  updateNTP();
  waitForSync();
  boolean succes = dateAndTime.setLocation(config.timezoneLocation);
  if (!succes) {
    // Panic!
  }
}

int previousRandomLed = 0;
void setRandomLed(CRGB color) {
  setAllPixels(CRGB::Black);

  int newRandom = previousRandomLed;
  while (newRandom == previousRandomLed) {
    newRandom = random(0, NUMBER_OF_LEDS);
  }
  previousRandomLed = newRandom;
  leds[newRandom] = color;
  FastLED.show();
}

int previousRandomPixel = 0;
void setRandomPixel(CRGB color) {
  setAllPixels(CRGB::Black);

  int newRandom = previousRandomPixel;
  while (newRandom == previousRandomPixel) {
    newRandom = random(0, NUMBER_OF_PIXELS);
  }
  previousRandomPixel = newRandom;
  setPixel(newRandom, color);
  FastLED.show();
}

void setClockPixels(byte hours, byte minutes) {  
  Serial.println("Set clock pixels");
  
  for (int i=0; i<NUMBER_OF_PIXELS; i++) {
    pixels[i] = 0;
  }
  
  setPixels(hours, 0x01);
  setPixels(minutes/5, 0x02);

  for (int i=0; i<NUMBER_OF_PIXELS; i++) {   
    setPixel(i, clockPalettes[config.clockPalette][pixels[i]]);
  }
  FastLED.show();
}

void setPixel(byte pixel, CRGB color) {
    switch(pixel) {
    case 0:
      // Pixel with value  1
      leds[11] = color;
      break;
    case 1:
      // Pixel with value 1
      leds[10] = color;
      break;
    case 2:
      // Pixel with value  2
      leds[9] = color;
      leds[8] = color;
      break;
    case 3:
      // Pixel with value  3
      leds[7] = color;
      leds[6] = color;
      leds[5] = color;
      break;
    case 4:
      // Pixel with value 5
      leds[4] = color;
      leds[3] = color;
      leds[2] = color;
      leds[1] = color;
      leds[0] = color;
      break;
  };
}

void onConnected(const WiFiEventStationModeConnected& event) {
  Serial.println("Connected to accesspoint, wating for ip");
}

void onGotIP(const WiFiEventStationModeGotIP& event) {
  Serial.print("Sucessfully obtained ip from ");
  Serial.println(WiFi.SSID());
  Serial.print("local-ip: ");
  Serial.println(WiFi.localIP());  

  startWebserver();
  connectedToNetwork = true;
}

void onDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Lost WiFi connection!");
  connectedToNetwork = false;
  // TODO: reconnect / error mode / ...
}
 
void setupNetwork() {
  gotIpEventHandler = WiFi.onStationModeGotIP(onGotIP);
  disconnectedEventHandler = WiFi.onStationModeDisconnected(onDisconnect);
  connectedEventHandler = WiFi.onStationModeConnected(onConnected);
  
  connectToWifi();
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi. Go into AccessPoint mode");
    setupAccessPoint();
    startCaptivePortalWebserver();
    setAllPixels(CRGB::Blue);
    FastLED.show();
  }
  
  Serial.print("host: ");
  Serial.println(host);
}

void setupAccessPoint() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(accesspointIp, accesspointIp, IPAddress(255, 255, 255, 0));

  // Please note that when the password is too short (less than 8 characters) the WiFi.softAP(ssid, password) function doesn't work. There is no warning during compilation.
  const char* accesspointPassword = "12345678";
  // TODO: In case of multiple clocks: add chip-id to be unique
  const char* accesspointSsid = "fibonacci-clock";
  
  boolean result = WiFi.softAP(accesspointSsid, accesspointPassword);
  result ? Serial.println("AccessPoint ready") : Serial.println("Failed to start AccessPoint");
  
  // if DNSServer is started with "*" for domain name, it will reply with provided IP to all DNS request
  const byte dnsPort = 53;
  dnsServer.start(dnsPort, "*", accesspointIp);

  Serial.print("accesspoint-Ssid: ");
  Serial.println(accesspointSsid);

  Serial.print("accesspoint-ip: ");
  Serial.println(WiFi.softAPIP());
}

void connectToWifi() {
  WiFi.persistent(false);
  WiFi.hostname(host);
  WiFi.mode(WIFI_STA);
  WiFi.begin(config.wifiSsid, config.wifiPassword);

  // Wait at most 15 seconds to connect
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
    Serial.write('.');
    setRandomPixel(CRGB::Yellow);
    delay(200);
  }
}

void startWebserver() {
  webserver.on("/", processRootRequest);
  webserver.on("/wifi", processWifiSettingsRequest);
  webserver.on("/settings", processSettingsRequest);
  webserver.onNotFound(handleNotFound);
  webserver.begin();
  Serial.println("Webserver started");
}

void startCaptivePortalWebserver() {
  webserver.onNotFound(processWifiSettingsRequest);
  webserver.begin();
  Serial.println("CaptivePortalWebserver started");
}

void setupBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT); 
  digitalWrite(BUZZER_PIN, LOW);
}

void setAllPixels(CRGB color) {
  for (int i=0; i<NUMBER_OF_PIXELS; i++) {
    setPixel(i, color);
  }
}

void setupFileSystem() {
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
  }
}
