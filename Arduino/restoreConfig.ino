boolean restoreConfig() {
  Serial.println("Reading EEPROM...");
  int c = 0;
  String ssid = "";
  String pass = "";
  String mqttServer = "";

  if (EEPROM.read(0) != 0) {
    for (int i = 0; i < 32; ++i) {
      ssid += char(EEPROM.read(i));
    }
    for (int i = 32; i < 96; ++i) {
      pass += char(EEPROM.read(i));
    }

    for (int i = 96; i < 112; i++)
    {
      mqttServer += char(EEPROM.read(i));
      mqtt_server[c] = mqttServer.charAt(c);
      c++;
    }



#ifdef DEBUG
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("mqtt_server: ");
    Serial.print(mqtt_server);
    Serial.print("mqttServer: ");
    Serial.print(mqttServer);
    Serial.print("Password: ");
    Serial.println(pass);
#endif

    WiFi.begin(ssid.c_str(), pass.c_str());
    return true;
  }
  else {
    Serial.println("Config not found.");
    return false;
  }
}

