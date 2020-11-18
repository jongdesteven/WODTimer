#include <Arduino.h>

#include "buttons.h"
#include "DisplayControl.h"
#include "TimerMenu.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//needed for WifiManager Library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

const char* ssid = "De Jong Airport";
const char* password = "Appeltaart";
const char* mqtt_server = "tinysrv";

WiFiClient espClient;
PubSubClient client(espClient);
WiFiManager wifiManager;

DisplayControl ledDisplay(15, 1);
TimerMenu cfTimer(ledDisplay);
PowerStartControlButton pwrBtn(4, cfTimer);
MenuControlButton menuBtn(12, cfTimer);
MinusButton minBtn(5, cfTimer);
PlusButton plusBtn(0, cfTimer);
char oldText2[6];

// MQTT start

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

 // WiFi.mode(WIFI_STA);
 // WiFi.begin(ssid, password);
 // while (WiFi.status() != WL_CONNECTED) {
 //   delay(500);
 //   Serial.print(".");
 // }
 //  randomSeed(micros());
  
  wifiManager.setConfigPortalBlocking(false);
	if(wifiManager.autoConnect("WODTimer_AP")){
		Serial.println("connected...:)");
		Serial.println("");
		Serial.println("WiFi connected");
		Serial.println("IP address: ");
		Serial.println(WiFi.localIP());
	}
	else {
		Serial.println("Configportal running");
	}

}

void callback(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Message arrived [");
  // Serial.print(topic);
  // Serial.print("] ");
  // for (int i = 0; i < length; i++) {
  //   Serial.print((char)payload[i]);
  // }
  // Serial.println();

  // Switch on the LED if an 1 was received as first character
  switch (atoi((char*)payload)){
  case 1:
    cfTimer.startTheTimer();
    break;
  case 2:
    cfTimer.advanceMenu();
    break;
  case 3:
    cfTimer.decrementOption();
    break;
  case 4:
    cfTimer.incrementOption();
    break;
  default:
    Serial.print("Button: Invalid: ");
    Serial.println(atoi((char*)payload));
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("wodtimer/debug", "connected");
      // ... and resubscribe
      client.subscribe("wodtimer/buttons");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
// MQTT End


void setup() {
  // put your setup code here, to run once:
  // MQTT Debug
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //MQTT Debug end

  // pwrBtn.setup();
  // menuBtn.setup();
  // minBtn.setup();
  // plusBtn.setup();
  cfTimer.setup();
  ledDisplay.setup();

}

void loop() {
  //MQTT debug
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (strcmp(oldText2, ledDisplay.getText()) != 0){
    client.publish("wodtimer/display", ledDisplay.getText());
    strcpy(oldText2, ledDisplay.getText());
  }
	
	wifiManage.process();
  //MQTT Debug end

  // pwrBtn.loop();
  // menuBtn.loop();
  // minBtn.loop();
  // plusBtn.loop();
  cfTimer.loop();
  ledDisplay.loop();
}
