void processRootRequest() {
  Serial.println("handleRoot");
  if (connectedToNetwork) {
    redirect("/settings");
  } else {
    redirect("/wifi");
  }
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
  String modeParam = getTrimmedWebserverArgument("mode");
  String clockPaletteParam = getTrimmedWebserverArgument("clockPalette");
  String brightnessParam = getTrimmedWebserverArgument("brightness");
  String lampColorHexParam = getTrimmedWebserverArgument("lampColorHex");
  String stroboColorHexParam = getTrimmedWebserverArgument("stroboColorHex");
  String stroboSpeedParam = getTrimmedWebserverArgument("stroboSpeed");
  String glowColorHexParam = getTrimmedWebserverArgument("glowColorHex");
  String glowSpeedParam = getTrimmedWebserverArgument("glowSpeed");
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

  config.mode = modeParam.toInt();
  config.brightness = brightnessParam.toInt();
  config.clockPalette = clockPaletteParam.toInt();
  config.lampColorHex = lampColorHexParam;
  config.lampColorR = lampR;
  config.lampColorG = lampG;
  config.lampColorB = lampB;
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

  saveConfiguration(configFileFullPath, config);
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
                "      function setVisibilityBasedOnMode() {\n"
                "        var mode = document.getElementById('mode').selectedIndex;\n"
                "        setVisibility('clockPaletteSetting', mode == 0);\n"
                "        setVisibility('clockPaletteLegend', mode == 0);\n"
                "        setVisibility('lampColorHexSetting', mode == 1 || mode == 2);\n"
                "        setVisibility('stroboColorHexSetting', mode == 3 || mode == 4 || mode == 5);\n"
                "        setVisibility('stroboSpeedSetting', mode == 3 || mode == 4 || mode == 5);\n"
                "        setVisibility('glowColorHexSetting', mode == 8);\n"
                "        setVisibility('glowSpeedSetting', mode == 8);\n"
                "        setVisibility('buzzerSecondSetting', mode == 0);\n"
                "        setVisibility('buzzerSecondFrequencySetting', mode == 0);\n"
                "        setVisibility('buzzerHalfHourSetting', mode == 0);\n"
                "        setVisibility('buzzerHourSetting', mode == 0);\n"
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
                "        setVisibilityBasedOnMode();"
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
                        "<select id='mode' name='mode' class='select' onchange='setVisibilityBasedOnMode()'>"
                          "<option value='0'" + selected(0, config.mode) + ">Clock</option>"
                          "<option value='1'" + selected(1, config.mode) + ">Lamp</option>"
                          "<option value='2'" + selected(2, config.mode) + ">Lamp with glitter</option>"
                          "<option value='3'" + selected(3, config.mode) + ">Strobo</option>"
                          "<option value='4'" + selected(4, config.mode) + ">Strobo (random pixel)</option>"
                          "<option value='5'" + selected(5, config.mode) + ">Strobo (random led)</option>"
                          "<option value='6'" + selected(6, config.mode) + ">Rainbow</option>"
                          "<option value='7'" + selected(7, config.mode) + ">Rainbow with glitter</option>"
                          "<option value='8'" + selected(8, config.mode) + ">Glow</option>"
                        "</select>"
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

void processWifiSettingsRequest() {
  if (webserver.method() == HTTP_GET) {
     getWifiSettings();
  } else if (webserver.method() == HTTP_POST) {
    postWifiSettings();
  }
}

void postWifiSettings() {  
  Serial.println("Post WiFi settings");
  
  String ssidParam = getTrimmedWebserverArgument("ssid");
  String passwordParam = getTrimmedWebserverArgument("password");

  Serial.println(ssidParam);
  Serial.println(passwordParam);

  if (ssidParam.length() == 0) {
    webserver.send(400, "text/html", "Failed to save configuration: ssid is mandatory");
    return;
  } else if (passwordParam.length() == 0) {
    webserver.send(400, "text/html", "Failed to save configuration: password is mandatory");
    return;
  }

  config.wifiSsid = ssidParam;
  config.wifiPassword = passwordParam;
  
  bool successfullySaved = saveConfiguration(configFileFullPath, config);
  if (!successfullySaved) {
    webserver.send(500, "text/html", "Failed to save configuration");
    return;
  }
  ESP.restart();
}

void getWifiSettings() {
  String html = "<!DOCTYPE html>"
                "<html>"
                "<head>"
                "  <title>Fibonacci clock</title>"
                "  <meta name='viewport' content='width=device-width, initial-scale=1'>"
                "  <link rel='stylesheet' type='text/css' href='main.css'>"
                "</head>"
                "<body>"
                "  <h2>WiFi configuration</h2>"
                "  <p>Fibonacci clock needs a WiFi network connection, <br/>please provide the SSID and password of the network in the form below.</p>"
                "  <form action='wifi' method='post'>"
                "  <table>"
                "    <tr>"
                "      <td>SSID * </td><td><input type='text' name='ssid' size='30' maxlength='50' required></td>"
                "    </tr>"
                "    <tr>"
                "      <td>Password * </td><td><input type='password' name='password' size='30' maxlength='50' required></td>"
                "    </tr>"
                "    <tr>"
                "      <td colspan='2' align='right'><input type='submit' value='Save and (re)connect'></td>"
                "    </tr>"
                 "  </table>"
                "</form>"
                "</body>"
                "</html>";
  webserver.send(200, "text/html", html);
}

void handleNotFound(){
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
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}
