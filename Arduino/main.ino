#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

//COMMENT THIS TO TURN OFF SERIAL PRINTS
#define DEBUG

void callback(char* topic, byte* payload, unsigned int length);

const IPAddress apIP(192, 168, 1, 1);
const char* apSSID = "Door Lock";
boolean settingMode;
String ssidList;

char mqtt_server[15] = {};//192.168.31.220
const char* mqtt_username = "Username goes here";
const char* mqtt_password = "Password goes here";
char const* switchTopic = "house/doorlock/";


//DOOR LOCK GPIO
const int lockPin = 2;

#define LOCK_HOLD_DOWN_TIME 2000 //in millisecond



long lastMsg = 0;
char msg[50];
int value = 0;


DNSServer dnsServer;
ESP8266WebServer webServer(80);
WiFiClient espClient;
PubSubClient client(mqtt_server, 1883, callback, espClient);



void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif

  EEPROM.begin(512);
  delay(10);
  pinMode(lockPin, INPUT_PULLUP);

#ifdef DEBUG
  Serial.println(F("------ DEBUG MODE ON------"));
  Serial.print(F("Digital pin 2 :  "));
  Serial.println(digitalRead(lockPin));
#endif

  //digitalWrite(lockPin, HIGH);

  if (restoreConfig()) {
    if (checkConnection()) {
      settingMode = false;
      pinMode(lockPin, OUTPUT);
      startWebServer();
      return;
    }
  }
  settingMode = true;
  setupMode();


}

void loop() {
  if (settingMode) {
    dnsServer.processNextRequest();
  }
  if (!client.connected() && WiFi.status() == 3) {
    reconnect();
  }

  client.loop();
  webServer.handleClient();
}

//CALLBACK
void callback(char* topic, byte* payload, unsigned int length) {
#ifdef DEBUG
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
#endif

  String topicStr = topic;
  //EJ: Note:  the "topic" value gets overwritten everytime it receives confirmation (callback) message from MQTT
#ifdef DEBUG
  //Print out some debugging info
  Serial.println(F("Callback update."));
  Serial.print(F("Topic: "));
  Serial.println(topicStr);
#endif

  if (topicStr == "house/doorlock/")
  {
    char p0 = (char)payload[0];

    //**************************** FOR PAYLOAD 1 ***************************************************
    //turn the switch on if the payload is '1' and publish to the MQTT server a confirmation message
    if (p0 == '1') {

#ifdef DEBUG
      Serial.println();
      Serial.println(F("LOCK ON"));
#endif
      //TURN ON LOCK
      digitalWrite(lockPin, LOW);//Inverted Logic

#ifdef DEBUG
      Serial.print(F("Lock Pin Status: ")); Serial.println(digitalRead(lockPin));
#endif
      client.publish("house/doorlock/RESULT", "1");
      delay(LOCK_HOLD_DOWN_TIME);
#ifdef DEBUG
      Serial.println(F("LOCK OFF"));//Inverted logic
#endif
      digitalWrite(lockPin, HIGH);
#ifdef DEBUG
      Serial.print(F("Lock Pin Status: ")); Serial.println(digitalRead(lockPin));
#endif
      client.publish("house/doorlock/RESULT", "0");

    }
    //****************************** PAYLOAD 1 END ************************************************



    //****************************** FOR PAYLOAD 0 ************************************************
    //turn the switch off if the payload is '0' and publish to the MQTT server a confirmation message
//    else if (p0 == '0') {
//      digitalWrite(2, HIGH);//Inverted logic
//      client.publish("house/doorlock/RESULT", "0");
//#ifdef DEBUG
//      Serial.println();
//      Serial.println(F("LOCK OFF"));
//      Serial.print(F("Lock Pin Status: ")); Serial.println(digitalRead(lockPin));
//#endif
//    }
    //****************************** PAYLOAD 0 END *************************************************
  }

}


