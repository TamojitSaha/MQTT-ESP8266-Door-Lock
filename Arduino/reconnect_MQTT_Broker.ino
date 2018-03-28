void reconnect() {
  // Loop until we're reconnected
  while (!client.connected())
  {
#ifdef DEBUG
    Serial.print("Attempting MQTT connection...");
#endif
    // Attempt to connect
    if (client.connect("ESP8266Client"))
    {
#ifdef DEBUG
      Serial.println("connected");
#endif
    }

    else
    {
#ifdef DEBUG
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
#endif
      // Wait 5 seconds before retrying
      delay(2000);
    }
  }
}
