void processRootRequest() {
  Serial.println("handleRoot");
  redirect("/settings");
}

void redirect(String toUrl) {
  webserver.sendHeader("Location", toUrl, true);
  webserver.send(302, "text/plain", "");
}

void processSettingsRequest() {
  if (webserver.method() == HTTP_GET) {
     getSettings();
  } else if (webserver.method() == HTTP_POST) {
    postSettings();
  }
}

void postSettings() {
  String pixelModeParam = getTrimmedWebserverArgument("pixelMode");
  String clockPaletteParam = getTrimmedWebserverArgument("clockPalette");
  String brightnessParam = getTrimmedWebserverArgument("brightness");
  String lampColorHexParam = getTrimmedWebserverArgument("lampColorHex");
  String stroboColorHexParam = getTrimmedWebserverArgument("stroboColorHex");
  String stroboSpeedParam = getTrimmedWebserverArgument("stroboSpeed");
  String glowColorHexParam = getTrimmedWebserverArgument("glowColorHex");
  String glowSpeedParam = getTrimmedWebserverArgument("glowSpeed");
  String rainbowSpeedParam = getTrimmedWebserverArgument("rainbowSpeed");
  String buzzerStartupParam = getTrimmedWebserverArgument("buzzerStartup");
  String buzzerSecondParam = getTrimmedWebserverArgument("buzzerSecond");
  String buzzerSecondFrequencyParam = getTrimmedWebserverArgument("buzzerSecondFrequency");
  String buzzerHalfHourParam = getTrimmedWebserverArgument("buzzerHalfHour");
  String buzzerHourParam = getTrimmedWebserverArgument("buzzerHour");
  
  long long lampColorNumber = strtoll( &lampColorHexParam[1], NULL, 16);
  long long lampR = lampColorNumber >> 16;
  long long lampG = lampColorNumber >> 8 & 0xFF;
  long long lampB = lampColorNumber & 0xFF;

  long long stroboColorNumber = strtoll( &stroboColorHexParam[1], NULL, 16);
  long long stroboR = stroboColorNumber >> 16;
  long long stroboG = stroboColorNumber >> 8 & 0xFF;
  long long stroboB = stroboColorNumber & 0xFF;
  
  long long glowColorNumber = strtoll( &glowColorHexParam[1], NULL, 16);
  long long glowR = glowColorNumber >> 16;
  long long glowG = glowColorNumber >> 8 & 0xFF;
  long long glowB = glowColorNumber & 0xFF;

  config.pixelMode = pixelModeParam.toInt();
  config.brightness = brightnessParam.toInt();
  config.clockPalette = clockPaletteParam.toInt();
  config.lampColorHex = lampColorHexParam;
  config.lampColorR = lampR;
  config.lampColorG = lampG;
  config.lampColorB = lampB;
  config.rainbowSpeed = rainbowSpeedParam.toInt();
  config.stroboColorHex = stroboColorHexParam;
  config.stroboColorR = stroboR;
  config.stroboColorG = stroboG;
  config.stroboColorB = stroboB;
  config.stroboSpeed = stroboSpeedParam.toInt();
  config.glowColorHex = glowColorHexParam;
  config.glowColorR = glowR;
  config.glowColorG = glowG;
  config.glowColorB = glowB;
  config.glowSpeed = glowSpeedParam.toInt();
  config.buzzerStartup = buzzerStartupParam.length() > 0 ? true : false;
  config.buzzerSecond = buzzerSecondParam.length() > 0 ? true : false;
  config.buzzerSecondFrequency = buzzerSecondFrequencyParam.toInt();
  config.buzzerHalfHour = buzzerHalfHourParam.length() > 0 ? true : false;
  config.buzzerHour = buzzerHourParam.toInt();

  String setDateTimeParam = getTrimmedWebserverArgument("setDateTime");
  
  saveConfiguration(configFileFullPath, config);

  if (setDateTimeParam.length() > 0) {
      String dateTime = getTrimmedWebserverArgument("dateTime");

      Serial.print("setDateTime: ");
      Serial.println(dateTime);

      int year = dateTime.substring(0,4).toInt();
      int month = dateTime.substring(5,7).toInt();
      int day = dateTime.substring(8,10).toInt();
      int hour = dateTime.substring(11,13).toInt();
      int minute = dateTime.substring(14,16).toInt();

      setTime(hour, minute, 0, day, month, year);
  }

  forceRefresh = true;
  redirect("/settings");
}

String getTrimmedWebserverArgument(String argName) {
  String argValue = webserver.arg(argName);
  argValue.trim();
  return argValue;
}

void getSettings() {
  String html = "<!DOCTYPE html>"
                "<html>"
                  "<head>"
                    "<title>Fibonacci clock</title>"
                    "<meta http-equiv='X-UA-Compatible' content='IE=edge'>"
                    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
                    "<meta name='mobile-web-app-capable' content='yes'>"
                    "<link rel='stylesheet' type='text/css' href='main.css'>"
                    "<link rel='apple-touch-icon' sizes='57x57' href='/icon/apple-icon-57x57.png'>"
                    "<link rel='apple-touch-icon' sizes='60x60' href='/icon/apple-icon-60x60.png'>"
                    "<link rel='apple-touch-icon' sizes='72x72' href='/icon/apple-icon-72x72.png'>"
                    "<link rel='apple-touch-icon' sizes='76x76' href='/icon/apple-icon-76x76.png'>"
                    "<link rel='apple-touch-icon' sizes='114x114' href='/icon/apple-icon-114x114.png'>"
                    "<link rel='apple-touch-icon' sizes='120x120' href='/icon/apple-icon-120x120.png'>"
                    "<link rel='apple-touch-icon' sizes='144x144' href='/icon/apple-icon-144x144.png'>"
                    "<link rel='apple-touch-icon' sizes='152x152' href='/icon/apple-icon-152x152.png'>"
                    "<link rel='apple-touch-icon' sizes='180x180' href='/icon/apple-icon-180x180.png'>"
                    "<link rel='icon' type='image/png' sizes='192x192' href='/icon/android-icon-192x192.png'>"
                    "<link rel='icon' type='image/png' sizes='32x32' href='/icon/favicon-32x32.png'>"
                    "<link rel='icon' type='image/png' sizes='96x96' href='/icon/favicon-96x96.png'>"
                    "<link rel='icon' type='image/png' sizes='16x16' href='/icon/favicon-16x16.png'>"
                    "<link rel='manifest' href='/icon/manifest.json'>"
                    "<meta name='msapplication-TileColor' content='#ffffff'>"
                    "<meta name='msapplication-TileImage' content='/icon/ms-icon-144x144.png'>"
                    "<meta name='theme-color' content='#ffffff'>"
                    "<script>\n"
                "      function setVisibility(id, visible) {\n"
                "        visible ? document.getElementById(id).style.display = '' : document.getElementById(id).style.display = 'none';\n"
                "      }\n"
                "      function setVisibilityBasedOnModes() {\n"
                "        var connectionMode = document.getElementById('connectionMode').selectedIndex;\n"
                "        var pixelMode = document.getElementById('pixelMode').selectedIndex;\n"
                "        setVisibility('clockPaletteSetting', pixelMode == 0);\n"
                "        setVisibility('clockDateTimeSetting', pixelMode == 0 && connectionMode == 0);\n"
                "        setVisibility('clockPaletteLegend', pixelMode == 0);\n"
                "        setVisibility('lampColorHexSetting', pixelMode == 1 || pixelMode == 2);\n"
                "        setVisibility('stroboColorHexSetting', pixelMode == 3 || pixelMode == 4 || pixelMode == 5);\n"
                "        setVisibility('stroboSpeedSetting', pixelMode == 3 || pixelMode == 4 || pixelMode == 5);\n"
                "        setVisibility('rainbowSpeedSetting', pixelMode == 6 || pixelMode == 7);\n"
                "        setVisibility('glowColorHexSetting', pixelMode == 8);\n"
                "        setVisibility('glowSpeedSetting', pixelMode == 8);\n"
                "        setVisibility('buzzerSecondSetting', pixelMode == 0);\n"
                "        setVisibility('buzzerSecondFrequencySetting', pixelMode == 0);\n"
                "        setVisibility('buzzerHalfHourSetting', pixelMode == 0);\n"
                "        setVisibility('buzzerHourSetting', pixelMode == 0);\n"
                "        setVisibility('ssidSetting', connectionMode == 1);\n"
                "        setVisibility('passwordSetting', connectionMode == 1);\n"
                "      }\n"
                "      function setBackground(element, color) {\n"
                "        document.getElementById(element).style.backgroundColor = color;\n"
                "      }\n"
                "      function setClockPaletteLegends(off, hours, minutes, both) {\n"
                "        setBackground('clockPaletteOff', off);\n"
                "        setBackground('clockPaletteHours', hours);\n"
                "        setBackground('clockPaletteMinutes', minutes);\n"
                "        setBackground('clockPaletteHoursMinutes', both);\n"
                "      }\n"                
                "      function setClockPaletteLegend() {\n"
                "        var selectedPalette = document.getElementById('clockPalette').value;\n";
        for (int i = 0; i < NUMBER_OF_CLOCK_COLOR_PALETTES; i++) {
          
        html += "        if (selectedPalette == " + String(i) + ") {\n"
                "          setClockPaletteLegends('rgb(" + clockPalettes[i][0].red + "," + clockPalettes[i][0].green + "," + clockPalettes[i][0].blue + ")', "
                                                 "'rgb(" + clockPalettes[i][1].red + "," + clockPalettes[i][1].green + "," + clockPalettes[i][1].blue + ")', "
                                                 "'rgb(" + clockPalettes[i][2].red + "," + clockPalettes[i][2].green + "," + clockPalettes[i][2].blue + ")', "
                                                 "'rgb(" + clockPalettes[i][3].red + "," + clockPalettes[i][3].green + "," + clockPalettes[i][3].blue + ")');\n"
                "        }\n";
        }
        html += "      }\n"
                "      function setup() {"
                "        setVisibilityBasedOnModes();"
                "        setClockPaletteLegend();"
                "      }"
                "      window.onload = setup;"
                "    </script>"
                "  </head>";
        html += "<body>"
                "<div class='container'>"
                  "<img class='logo' src='logo.png' alt='Fibonacci' width='285'>"
                  
                    "<form action='settings' method='post'>"  
                    "<fieldset>"
                    "<legend>Light</legend>"
                    "<div>"
                      "<label>Brightness (1% &harr; 100%):</label>"
                        "<input class='slider' name='brightness' type='range' min='1' max='255' value='" + String(config.brightness) + "'>"
                      "<div>"
                      "<div class='control'>"
                        "<label>Mode:</label>"
                        "<select id='pixelMode' name='pixelMode' class='select' onchange='setVisibilityBasedOnModes()'>"
                          "<option value='0'" + selected(0, config.pixelMode) + ">Clock</option>"
                          "<option value='1'" + selected(1, config.pixelMode) + ">Lamp</option>"
                          "<option value='2'" + selected(2, config.pixelMode) + ">Lamp with glitter</option>"
                          "<option value='3'" + selected(3, config.pixelMode) + ">Strobo</option>"
                          "<option value='4'" + selected(4, config.pixelMode) + ">Strobo (random pixel)</option>"
                          "<option value='5'" + selected(5, config.pixelMode) + ">Strobo (random led)</option>"
                          "<option value='6'" + selected(6, config.pixelMode) + ">Rainbow</option>"
                          "<option value='7'" + selected(7, config.pixelMode) + ">Rainbow with glitter</option>"
                          "<option value='8'" + selected(8, config.pixelMode) + ">Glow</option>"
                        "</select>"
                      "</div>"

                      "<div class='control' id='clockDateTimeSetting' style='display:none'>"
                        "<input name='setDateTime' type='checkbox'> change date/time to:"
                        "<input type='datetime-local' class='datetime-local' name='dateTime' value='" + dateTime("Y-m-d") + "T" + dateTime("H:i")  + "'>"
                      "</div>"

                      "<div class='control' id='clockPaletteSetting' style='display:none'>"
                        "<label>Colorscheme:</label>"
                        "<select id='clockPalette' name='clockPalette' class='select' onchange='setClockPaletteLegend()'>"
                          "<option value='0'" + selected(0, config.clockPalette) + ">RGB</option>"
                          "<option value='1'" + selected(1, config.clockPalette) + ">Mondrian</option>"
                          "<option value='2'" + selected(2, config.clockPalette) + ">Basbrun</option>"
                          "<option value='3'" + selected(3, config.clockPalette) + ">80's</option>"
                          "<option value='4'" + selected(4, config.clockPalette) + ">Pastel</option>"
                          "<option value='5'" + selected(5, config.clockPalette) + ">Modern</option>"
                          "<option value='6'" + selected(6, config.clockPalette) + ">Cold</option>"
                          "<option value='7'" + selected(7, config.clockPalette) + ">Warm</option>"
                          "<option value='8'" + selected(8, config.clockPalette) + ">Earth</option>"
                          "<option value='9'" + selected(9, config.clockPalette) + ">Dark</option>"
                          "<option value='10'" + selected(10, config.clockPalette) + ">Dutch flag</option>"
                        "</select>"
                      "</div>"
                      "<div class='control' id='clockPaletteLegend' style='display:none'>"
                        "<div class='flex-container'>"
                          "<div class='clockPaletteItem' id='clockPaletteHours' title='Hours' style='background-color: white;'></div>"
                          "<div class='clockPaletteItem' id='clockPaletteMinutes' title='Minutes' style='background-color: white;'></div>"
                          "<div class='clockPaletteItem' id='clockPaletteHoursMinutes' title='Hours and minutes' style='background-color: white;'></div>"
                          "<div class='clockPaletteItem' id='clockPaletteOff' title='N/A' style='background-color: white;'></div>"
                        "</div>"
                      "</div>"
                      "<div class='control' id='rainbowSpeedSetting' style='display:none'>"
                        "<label>Speed (fast &harr; slow):</label>"
                        "<input class='slider' name='rainbowSpeed' type='range' min='1' max='2000' value='" + String(config.rainbowSpeed) + "'>"
                      "</div>"
                      "<div class='control' id='lampColorHexSetting' style='display:none'>"
                        "<label>Lamp color:</label>"
                        "<input class='color' type='color' name='lampColorHex' value='" + config.lampColorHex + "'>"
                      "</div>"
                      "<div class='control' id='stroboColorHexSetting' style='display:none'>"
                        "<label>Color:<label>"
                        "<input class='color' type='color' name='stroboColorHex' value='" + config.stroboColorHex + "'>"
                      "</div>"
                      "<div class='control' id='stroboSpeedSetting' style='display:none'>"
                        "<label>Speed (fast &harr; slow):</label>"
                        "<input class='slider' name='stroboSpeed' type='range' min='20' max='1000' value='" + String(config.stroboSpeed) + "'>"
                      "</div>"
                      "<div class='control' id='glowColorHexSetting' style='display:none'>"
                        "<label>Color:</label>"
                        "<input class='color' type='color' name='glowColorHex' value='" + config.glowColorHex + "'>"
                      "</div>"
                      "<div class='control' id='glowSpeedSetting' style='display:none'>"
                        "<label>Speed (fast &harr; slow):</label>"
                        "<input class='slider' name='glowSpeed' type='range' min='20' max='100' value='" + String(config.glowSpeed) + "'>"
                      "</div>"
                    "</fieldset>"
                    "<fieldset>"
                      "<legend>Sound</legend>"
                      "<div class='control'>"
                        "<input name='buzzerStartup' type='checkbox' " + checked(config.buzzerStartup) + "> startup"
                      "</div>"
                      "<div class='control' id='buzzerSecondSetting' style='display:none'>"
                        "<input name='buzzerSecond' type='checkbox' " + checked(config.buzzerSecond) + "> second"
                      "</div>"
                      "<div class='control' id='buzzerSecondFrequencySetting' style='display:none'>"
                        "<label>Second frequency (low &harr; high):</label>"
                        "<input class='slider' name='buzzerSecondFrequency' type='range' min='31' max='5000' value='" + String(config.buzzerSecondFrequency) + "'>"
                      "</div>"
                      "<div class='control' id='buzzerHalfHourSetting' style='display:none'>"
                        "<input name='buzzerHalfHour' type='checkbox' " + checked(config.buzzerHalfHour) + "> half hour"
                      "</div>"
                      "<div class='control' id='buzzerHourSetting' style='display:none'>"
                        "<label>Hour:</label>"
                        "<select name='buzzerHour' class='select'>"
                          "<option value='0'" + selected(0, config.buzzerHour) + ">Off</option>"
                          "<option value='1'" + selected(1, config.buzzerHour) + ">Countdown</option>"
                          "<option value='2'" + selected(2, config.buzzerHour) + ">Cuckoo</option>"
                        "</select>"
                      "</div>"
                      "</fieldset>"
                      "<div class='control' align='right'>"
                        "<input class='button' type='submit' value='Save and apply'>"
                      "</div>"
                  "</form>"
                  
                  "<br/>"
                  
                  "<form action='connection' method='post'>"
                    "<fieldset>"
                    "<legend>Connection</legend>"
                    "<div>"
                        "<select id='connectionMode' name='connectionMode' class='select' onchange='setVisibilityBasedOnModes()'>"
                          "<option value='0'" + selected(0, config.connectionMode) + ">Access Point (AP)</option>"
                          "<option value='1'" + selected(1, config.connectionMode) + ">Station (STA)</option>"
                        "</select>"
                    "</div>"
                    "<div class='control' id='ssidSetting' style='display:none'>"
                      "<label>SSID:</label>"
                      "<input type='text' class='text' id='ssid' name='ssid' size='30' maxlength='50' value='" + config.wifiSsid + "'>"
                    "</div>"
                    "<div class='control' id='passwordSetting' style='display:none'>"
                      "<label>Password:</label>"
                      "<input type='password' class='text' id='password' name='password' size='30' maxlength='50' value='" + config.wifiPassword + "'>"
                    "</div>"
                    "<div class='control' align='right'>"
                      "<input class='button' type='submit' value='Save and apply'>"
                    "</div>"
                    "</fieldset>"
                    "</form>"
                    
                "</div>"
                "</body>"
                "</html>";

    webserver.send(200, "text/html", html);
}

String selected(byte value, byte othervalue) {
  return value == othervalue ? " selected" : "";
}

String checked(boolean value) {
  return value ? " checked" : "";
}

void processConnectionSettingsRequest() {
  if (webserver.method() == HTTP_GET) {
     getSettings();
  } else if (webserver.method() == HTTP_POST) {
    postConnectionSettings();
  }
}

void postConnectionSettings() {  
  Serial.println("Post connection settings");

  byte connectionMode = getTrimmedWebserverArgument("connectionMode").toInt();
  boolean connectionModeChanged = config.connectionMode != connectionMode;
  
  String ssidParam = getTrimmedWebserverArgument("ssid");
  String passwordParam = getTrimmedWebserverArgument("password");

  if (connectionMode == CONNECTION_MODE_STA) {
    if (ssidParam.length() == 0) {
      webserver.send(400, "text/html", "Failed to save configuration: ssid is mandatory for connectionMode STA");
      return;
    } else if (passwordParam.length() == 0) {
      webserver.send(400, "text/html", "Failed to save configuration: password is mandatory for connectionMode STA");
      return;
    }    
  }

  config.connectionMode = connectionMode;
  config.wifiSsid = ssidParam;
  config.wifiPassword = passwordParam;

  bool successfullySaved = saveConfiguration(configFileFullPath, config);
  if (!successfullySaved) {
    webserver.send(500, "text/html", "Failed to save configuration");
    return;
  }
  setAllPixels(CRGB::Black);
  FastLED.show();

  ESP.restart();
}

void handleNotFound() {
  if (webserver.method() == HTTP_GET) {
    if (handleFileRead(webserver.uri())) {
      return;
    }
  }
  String message = "Not Found\n\n";
  message += "URI: ";
  message += webserver.uri();
  message += "\nMethod: ";
  message += (webserver.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webserver.args();
  message += "\n";
  for (int i=0; i<webserver.args(); i++){
    message += " " + webserver.argName(i) + ": " + webserver.arg(i) + "\n";
  }
  webserver.send(404, "text/plain", message);
}

bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);

  webserver.sendHeader("Cache-Control","public, max-age=2628000");
  
  String contentType = getContentType(path);
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    size_t sent = webserver.streamFile(file, contentType);
    file.close();
    return true;
  }
  Serial.println("File Not Found!");
  return false;
}

String getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".js")) return "text/javascript";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}
