#include <cstddef>
#include "web.h"
#include "datahandler.h"
#include "config.h"
#include <HardwareSerial.h>
#include <ESP8266WebServer.h>

extern CLUSTER_DATA cluster_data;
extern uint8_t shift_values;

ESP8266WebServer server(HTTP_PORT); 

void addRoutes() {
  server.on("/updateTelemetry", updateTelemetry);
  server.on("/shiftState", shiftState);
  server.begin();
}

void updateTelemetry() {
  for (int i = 0; i < server.args(); i++) {
    switch (server.argName(i)[0]) {
      case 's':
      {
        updateSpeedoDelay(atoi(server.arg(i).c_str()));
        break;
      }
      case 't':
      {
        updateTachoDelay(atoi(server.arg(i).c_str()));
        break;
      }
      case 'f':
      {
        updateFuelAnalogVal(atoi(server.arg(i).c_str()));
        break;
      }
    }
  }
  String response = 
    "Speedo: " + (String)cluster_data.speedoDelay +
    "\nTacho: " + (String)cluster_data.tachoDelay +
    "\nFuel: " + (String)cluster_data.fuelAnalogVal;
  
  server.send(200, "text/plain", response);
  
} 

void shiftState() {
  String bitstate = "";
  for(int i = 0; i < 8; i++) {
    bitstate += bitRead(shift_values, i);
  }
  server.send(200, "text/plain", bitstate);
}

void handleClient() {
  server.handleClient();
}