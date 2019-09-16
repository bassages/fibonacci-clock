// Loads the configuration from a file
void loadConfiguration(const char *filename, Config &config) {
  File file = SPIFFS.open(filename, "r");

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<1024> doc;

  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println("Failed to read file, using default configuration");
  }
  
  config.wifiSsid = doc["wifiSsid"] | "default";
  config.wifiPassword = doc["wifiPassword"] | "default";
  config.pixelMode = doc["pixelMode"] | 0;
  config.brightness = doc["brightness"] | 127;
  config.clockPalette = doc["palette"] | 0;
  config.rainbowSpeed = doc["rainbowSpeed"] | 80;
  config.lampColorHex = doc["lampColorHex"] | "#FFFF00";
  config.lampColorR = doc["lampColorR"] | 255;
  config.lampColorG = doc["lampColorG"] | 255;
  config.lampColorB = doc["lampColorB"] | 0;
  config.stroboColorHex = doc["stroboColorHex"] | "#FFFFFF";
  config.stroboColorR = doc["stroboColorR"] | 255;
  config.stroboColorG = doc["stroboColorG"] | 255;
  config.stroboColorB = doc["stroboColorB"] | 255;
  config.stroboSpeed = doc["stroboSpeed"] | 100;
  config.glowColorHex = doc["glowColorHex"] | "#FF0000";
  config.glowColorR = doc["glowColorR"] | 255;
  config.glowColorG = doc["glowColorG"] | 0;
  config.glowColorB = doc["glowColorB"] | 0;
  config.glowSpeed = doc["glowSpeed"] | 10;
  config.buzzerStartup = doc["buzzerStartup"] | true;
  config.buzzerSecond = doc["buzzerSecond"] | false;
  config.buzzerSecondFrequency = doc["buzzerSecondFrequency"] | 440;
  config.buzzerHalfHour = doc["buzzerHalfHour"] | true;
  config.buzzerHour = doc["buzzerHour"] | 1;
  config.timezoneLocation = doc["timezoneLocation"] | "Europe/Amsterdam";
  config.connectionMode = doc["connectionMode"] | CONNECTION_MODE_STA;
  file.close();
  printConfig();
}

boolean saveConfiguration(const char *filename, const Config &config) {
  Serial.println("Saving configuration");
  
  File configFile = SPIFFS.open(filename, "w");
  if (!configFile) {
    Serial.print("Failed to open ");
    Serial.print(configFileFullPath);
    Serial.println(" for writing");
    return false;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<1024> doc;

  doc["wifiSsid"] = config.wifiSsid;
  doc["wifiPassword"] = config.wifiPassword;
  doc["connectionMode"] = config.connectionMode;
  doc["brightness"] = config.brightness;
  doc["pixelMode"] = config.pixelMode;
  doc["palette"] = config.clockPalette;
  doc["rainbowSpeed"] = config.rainbowSpeed;
  doc["lampColorR"] = config.lampColorR;
  doc["lampColorG"] = config.lampColorG;
  doc["lampColorB"] = config.lampColorB;
  doc["lampColorHex"] = config.lampColorHex;
  doc["stroboColorR"] = config.stroboColorR;
  doc["stroboColorG"] = config.stroboColorG;
  doc["stroboColorB"] = config.stroboColorB;
  doc["stroboColorHex"] = config.stroboColorHex;
  doc["stroboSpeed"] = config.stroboSpeed;
  doc["glowColorR"] = config.glowColorR;
  doc["glowColorG"] = config.glowColorG;
  doc["glowColorB"] = config.glowColorB;
  doc["glowColorHex"] = config.glowColorHex;
  doc["glowSpeed"] = config.glowSpeed;
  doc["buzzerStartup"] = config.buzzerStartup;
  doc["buzzerSecond"] = config.buzzerSecond;
  doc["buzzerHalfHour"] = config.buzzerHalfHour;
  doc["buzzerHour"] = config.buzzerHour;
  doc["timezoneLocation"] = config.timezoneLocation;
  
  if (serializeJson(doc, configFile) == 0) {
    Serial.println("Failed to write to file");
  }
  configFile.close();

  printConfig();
  Serial.println("Configuration was saved successfully");
}

void printConfig() {
  Serial.println("----- <configuration> -----");
  Serial.print("wifiSsid: ");
  Serial.println(config.wifiSsid);

  Serial.print("wifiPassword: ");
  Serial.println(config.wifiPassword);
  
  Serial.print("brightness: ");
  Serial.println(config.brightness);

  Serial.print("connectionMode: ");
  Serial.println(config.connectionMode);

  Serial.print("pixelMode: ");
  Serial.println(config.pixelMode);

  Serial.print("clockPalette: ");
  Serial.println(config.clockPalette);

  Serial.print("rainbowSpeed: ");
  Serial.println(config.rainbowSpeed);
  
  Serial.print("lampColor (R,G,B / hex): ");
  Serial.print(config.lampColorR);
  Serial.print(",");
  Serial.print(config.lampColorG);
  Serial.print(",");
  Serial.print(config.lampColorB);
  Serial.print(" / ");
  Serial.println(config.lampColorHex);

  Serial.print("stroboColor (R,G,B / hex): ");
  Serial.print(config.stroboColorR);
  Serial.print(",");
  Serial.print(config.stroboColorG);
  Serial.print(",");
  Serial.print(config.stroboColorB);
  Serial.print(" / ");
  Serial.println(config.stroboColorHex);
  Serial.print("stroboSpeed: ");
  Serial.println(config.stroboSpeed);

  Serial.print("glowColor (R,G,B / hex): ");
  Serial.print(config.glowColorR);
  Serial.print(",");
  Serial.print(config.glowColorG);
  Serial.print(",");
  Serial.print(config.glowColorB);
  Serial.print(" / ");
  Serial.println(config.glowColorHex);
  Serial.print("glowSpeed: ");
  Serial.println(config.glowSpeed);

  Serial.print("buzzerStartup: ");
  Serial.println(config.buzzerStartup);

  Serial.print("buzzerSecond: ");
  Serial.println(config.buzzerSecond);

  Serial.print("buzzerHalfHour: ");
  Serial.println(config.buzzerHalfHour);

  Serial.print("buzzerHour: ");
  Serial.println(config.buzzerHour);
  
  Serial.print("timezoneLocation: ");
  Serial.println(config.timezoneLocation);

  Serial.println("----- <end configuration> -----");
}
