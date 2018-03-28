void startWebServer() {
  if (settingMode) {
    Serial.print("Starting Web Server at ");
    Serial.println(WiFi.softAPIP());
    webServer.on("/settings", []() {
      String s = "<h1>Wi-Fi Settings</h1><p>Please enter your password by selecting the SSID.</p>";
      s += "<form method=\"get\" action=\"setap\"><label>SSID: </label><select name=\"ssid\">";
      s += ssidList;
      s += "</select><br>Password: <input name=\"pass\" length=64 type=\"password\">";
      s += "<br>MQTT Server: <input name=\"server\" length=15 placeholder=\"xxx.xxx.xxx.xxx\"><br>";
      s += "<input type=\"submit\"></form>";
      webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
    });
    webServer.on("/setap", []() {
      for (int i = 0; i < 111; ++i) {
        EEPROM.write(i, 0);
      }
      String broker = urlDecode(webServer.arg("server"));
      String ssid = urlDecode(webServer.arg("ssid"));
      String pass = urlDecode(webServer.arg("pass"));

#ifdef DEBUG
      Serial.print(F("SSID: "));
      Serial.println(ssid);
      Serial.print(F("Password: "));
      Serial.println(pass);
      Serial.print(F("Broker: "));
      Serial.println(broker);
      Serial.println(F("Writing SSID to EEPROM..."));
#endif
      for (int i = 0; i < ssid.length(); ++i)
      {
        EEPROM.write(i, ssid[i]);
      }
#ifdef DEBUG
      Serial.println(F("Writing Password to EEPROM..."));
#endif

      for (int i = 0; i < pass.length(); ++i)
      {
        EEPROM.write(32 + i, pass[i]);
      }

#ifdef DEBUG
      Serial.println(F("Writing Broker to EEPROM..."));
#endif
      for (int i = 0; i < broker.length(); i++)
      {
        EEPROM.write(96 + i, broker[i]);
      }

      EEPROM.commit();
      Serial.println("Write EEPROM done!");
      String s = "<h1>Setup complete.</h1><p>device will be connected to \"";
      s += ssid;
      s += "\" after the restart.";
      webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
      ESP.restart();
    });
    webServer.onNotFound([]() {
      String s = "<h1>AP mode</h1><p><a href=\"/settings\">Wi-Fi Settings</a></p>";
      webServer.send(200, "text/html", makePage("AP mode", s));
    });
  }
  else {
    Serial.print("Starting Web Server at ");
    Serial.println(WiFi.localIP());
    webServer.on("/", []() {
      String s = "<h1>STA mode</h1><p><a href=\"/reset\">Reset Wi-Fi Settings</a></p>";
      webServer.send(200, "text/html", makePage("STA mode", s));
    });
    webServer.on("/reset", []() {
      for (int i = 0; i < 96; ++i) {
        EEPROM.write(i, 0);
      }
      EEPROM.commit();
      String s = "<h1>Wi-Fi settings was reset.</h1><p>Please reset device.</p>";
      webServer.send(200, "text/html", makePage("Reset Wi-Fi Settings", s));
    });
  }
  webServer.begin();
}

