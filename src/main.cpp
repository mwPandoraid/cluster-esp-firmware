#include <ArduinoOTA.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include "datahandler.h"
#include "web.h"
#include "config.h"

#define EEPROM_SIZE 68

typedef struct {
  char NETWORK_SSID[32];
  char NETWORK_PWD[32];
  union {
    // honestly not sure what i need this for but i had a cool idea so i did it
    uint32_t ip;
    struct {
      uint8_t q1;
      uint8_t q2;
      uint8_t q3;
      uint8_t q4;
    };
  };
} NETWORK_INFO;

CLUSTER_DATA cluster_data;

void getNetInfo(NETWORK_INFO* net_info);
void setNetInfo(NETWORK_INFO* net_info);
void eraseNetInfo();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("----ESP8266 AUDI CLUSTER FIRMWARE SERIAL----");
  Serial.println("Booting...");

  pinMode(SHIFT_CLK_PIN, OUTPUT);
  pinMode(SHIFT_DATA_PIN, OUTPUT);
  pinMode(0, OUTPUT);

  Serial.println("Initializing mock eeprom using flash memory with size " + String(EEPROM_SIZE + 1));
  EEPROM.begin(EEPROM_SIZE + 1);

  const int AP_MODE = EEPROM.read(EEPROM_SIZE);

  if(AP_MODE == 1) { 
    Serial.println("Skipping network load, going into AP mode.");
    Serial.println("MOCK - PUSHING WIFI DATA TO EEPROM");

    uint8_t address_space[EEPROM_SIZE];
    memset(address_space, 0, EEPROM_SIZE);

    NETWORK_INFO* net_info = (NETWORK_INFO*)address_space;

    // TODO: implement a proper website to input WiFi details
    // for now this is just a mock feature

    strncpy(net_info->NETWORK_SSID, "REDACTED", 32);
    strncpy(net_info->NETWORK_PWD, "REDACTED", 32);

    /*
    net_info->q1 = 192;
    net_info->q2 = 168;
    net_info->q3 = 1;
    net_info->q4 = 21;
    */

    Serial.println(net_info->NETWORK_SSID);
    Serial.println(net_info->NETWORK_PWD);

    setNetInfo(net_info);
    
    EEPROM.put(EEPROM_SIZE, 0);
    EEPROM.commit();
    ESP.restart();
  } else {
    Serial.println("Fetching network info from EEPROM");

    uint8_t address_space[EEPROM_SIZE];
    memset(address_space, 0, EEPROM_SIZE);

    NETWORK_INFO* net_info = (NETWORK_INFO*)address_space;
    getNetInfo(net_info);

    Serial.println("Network SSID: " + String(net_info->NETWORK_SSID) + "\nNetwork PWD: " + String(net_info->NETWORK_PWD));
    WiFi.begin(net_info->NETWORK_SSID, net_info->NETWORK_PWD);
    
    Serial.println("Connecting to network...");
    int iters = 0;

    while(WiFi.status() != WL_CONNECTED) {

      delay(500);

      Serial.print('.');
      iters++;
      if(WiFi.status() == WL_CONNECT_FAILED || iters == 48) {

        Serial.println("CONNECTION TO WIFI FAILED!!! GOING INTO AP MODE");

        EEPROM.put(EEPROM_SIZE, 1);
        EEPROM.commit();
        ESP.restart();
      }
    }

    Serial.println("Successfully connected to WIFI");
    Serial.println(WiFi.localIP());
    Serial.println("Initializing OTA...");
    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    addRoutes();
  }
}

void loop() {
  ArduinoOTA.handle();
  handleData();
  handleClient();
}

// maybe move this to a different file?

void getNetInfo(NETWORK_INFO* net_info) {
  EEPROM.get(0, *net_info);
}

void setNetInfo(NETWORK_INFO* net_info) {
  EEPROM.put(0, *net_info);
  EEPROM.commit();
}

void eraseNetInfo() {
  char buffer[EEPROM_SIZE] = { 0 };
  EEPROM.put(0, buffer);
}