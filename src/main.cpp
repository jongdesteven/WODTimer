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

// Classes
#include "MainMenu.h"
#include "buttons.h"
#include "DisplayControl.h"

char hostname[16];
const char* boardName = "WODTimer";
const char* mqtt_server = "tinysrv";

WiFiClient espClient;
PubSubClient client(espClient);

DisplayControl ledDisplay(15, 1);
MainMenu mainMenu(ledDisplay);
PowerStartControlButton pwrBtn(4, mainMenu);
MenuControlButton menuBtn(12, mainMenu);
MinusButton minBtn(5, mainMenu);
PlusButton plusBtn(0, mainMenu); 
char oldText2[6];

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(SSID_INFO);
  Serial.print(" as ");
  Serial.println(hostname);

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
    // mainMenu.startTheTimer();
    mainMenu.powerAction();
    break;
  case 2:
    // mainMenu.advanceMenu();
    mainMenu.menuAction();
    break;
  case 3:
    //mainMenu.decrementOption();
    mainMenu.decrementAction();
    break;
  case 4:
    //mainMenu.incrementOption();
    mainMenu.incrementAction();
    break;
  case 5:
    mainMenu.returnAction();
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
    String clientId = hostname;
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

bool wakeFromDeepSleep(){
  pinMode(D3, INPUT_PULLUP);
  if (digitalRead(D3) == LOW) {
    return true;
  }
  else {
    return false;
  }
}

void setup() {

  Serial.begin(115200);
  if (!wakeFromDeepSleep()){
    Serial.println("Button D3/GPIO0 not pressed, going to DeepSleep for 3s");
    ESP.deepSleep(3*1000000);
  }

  sprintf(hostname, "%s-%06x", boardName,  ESP.getChipId());
  setup_wifi();
  setup_OTA();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // pwrBtn.setup();
  // menuBtn.setup();
  // minBtn.setup();
  // plusBtn.setup();
  mainMenu.setup();
  //ledDisplay.setup();
}

void loop() {
  ArduinoOTA.handle();  
  // pwrBtn.loop();
  // menuBtn.loop();
  // minBtn.loop();
  // plusBtn.loop();
  mainMenu.loop();
  ledDisplay.loop();
  sendMQTTStatus();
}
