#include <Arduino.h>
#include "ssid_info.h"

// For ArduinoOTA
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// For MQTT
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//needed for WifiManager Library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

// Classes
#include "buttons.h"
#include "DisplayControl.h"
#include "TimerMenu.h"

char hostname[16];
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

void setup_wifiManager() {
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

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID_INFO);

  WiFi.hostname(hostname);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_INFO, SSID_INFO_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  randomSeed(micros());
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());	
}

void callback(char* topic, byte* payload, unsigned int length) {
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

void mqtt_reconnect() {
  if (!client.connected()) {
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

void setup_OTA(){
  ArduinoOTA.setHostname(hostname);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");
  
ArduinoOTA.onStart([]() {
	String type;
	if (ArduinoOTA.getCommand() == U_FLASH) {
		type = "sketch";
	} else { // U_FS
		type = "filesystem";
	}
	// NOTE: if updating FS this would be the place to unmount FS using FS.end()
	Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}

void sendMQTTStatus(){
  if (WiFi.status() == WL_CONNECTED){
    if (!client.connected()) {
      mqtt_reconnect();
    }
    client.loop();
    if (strcmp(oldText2, ledDisplay.getText()) != 0){
      client.publish("wodtimer/display", ledDisplay.getText());
      strcpy(oldText2, ledDisplay.getText());
    }
  }
}

void setup() {
  sprintf(hostname, "WODTimer-%06x", ESP.getChipId());
  Serial.begin(115200);
  setup_wifi();
  setup_OTA();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // pwrBtn.setup();
  // menuBtn.setup();
  // minBtn.setup();
  // plusBtn.setup();
  cfTimer.setup();
  //ledDisplay.setup();
}

void loop() {
  //wifiManager.process();
  ArduinoOTA.handle();  
  // pwrBtn.loop();
  // menuBtn.loop();
  // minBtn.loop();
  // plusBtn.loop();
  cfTimer.loop();
  ledDisplay.loop();
  sendMQTTStatus();
}
