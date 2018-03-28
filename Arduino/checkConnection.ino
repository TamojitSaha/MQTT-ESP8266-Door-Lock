boolean checkConnection() {
  int count = 0;
  Serial.print("Waiting for Wi-Fi connection");
  while ( count < 30 ) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("Connected!");

      while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");

        // Generate client name based on MAC address and last 8 bits of microsecond counter
        String clientName;
        clientName += "esp8266-";
        uint8_t mac[6];
        WiFi.macAddress(mac);
        clientName += macToStr(mac);

        //if connected, subscribe to the topic(s) we want to be notified about
        //EJ: Delete "mqtt_username", and "mqtt_password" here if you are not using any
        if (client.connect((char*) clientName.c_str(), mqtt_username, mqtt_password)) { //EJ: Update accordingly with your MQTT account
          Serial.print("\tMQTT Connected");
          client.subscribe(switchTopic);
          //EJ: Do not forget to replicate the above line if you will have more than the above number of relay switches
        }

        //otherwise print failed for debugging
        else {
          Serial.println("\tFailed.");
          abort();
        }
      }
      return (true);
    }
    delay(500);
    Serial.print(".");
    count++;
  }
  Serial.println("Timed out.");
  return false;
}

String macToStr(const uint8_t* mac){

  String result;

  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);

    if (i < 5){
      result += ':';
    }
  }

  return result;
}
