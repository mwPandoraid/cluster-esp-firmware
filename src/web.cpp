#include <cstddef>
#include "web.h"
#include "datahandler.h"
#include "config.h"
#include <HardwareSerial.h>
#include <ESP8266WebServer.h>

extern CLUSTER_DATA cluster_data;
extern uint8_t shift_values;

ESP8266WebServer server(80); 

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
        update_speed_delay(atoi(server.arg(i).c_str()));
        break;
      }
      case 't':
      {
        update_tacho_delay(atoi(server.arg(i).c_str()));
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

/*

void processIncomingByte(const unsigned char inByte)
{
  static char input_line[50];
  static unsigned int input_pos = 0;

  switch (inByte)
  {

  case '\n':                   // end of text
    input_line[input_pos] = 0; // terminating null byte

    // terminator reached! process input_line here ...
    process_data(input_line);

    // reset buffer for next time
    input_pos = 0;
    break;

  case '\r': // discard carriage return
    break;

  default:
    // keep adding if not full ... allow for terminating null byte
    if (input_pos < (50 - 1))
      input_line[input_pos++] = inByte;
    break;

  } // end of switch
}


void process_data(const char *data)
{
  // for now just display it
  // (but you could compare it to some value, convert to an integer, etc.)
  Serial.println(data);
  int currSpeed;
  int currRPM;
  switch (data[0])
  {
  case 's':
  {
    data++;

    sscanf(data, "%d", &currSpeed);
    
    update_speed_delay()
  }
  break;

  case 't':
  {
    data++;

    sscanf(data, "%d", &currRPM);

    float frequencyT = currRPM * 0.035;
    float periodT = 1 / frequencyT;

    tachodelay = periodT * 1000000;
    Serial.println(tachodelay);
  }
  break;

  case 'f':

    data++;
    sscanf(data, "%d", &currFuel);

    fuelDutyCycle = map(currFuel, 0, 100, 0, 75);

    break;

  case 'b':
  {
    data++;

    int state = data[1] == '1' ? 1 : 0;

    if (data[0] == 'l')
    {
      digitalWrite(lBlinker, state);
    }

    else
    {
      digitalWrite(rBlinker, state);
    }
  }
  break;

  case 'w':

    data++;
    sscanf(data, "%d", &currTemp);

    currTemp = round(currTemp / 10) * 10;

    switch (currTemp)
    {
    case 50:
      tempDutyCycle = 3;
      break;
    case 60:
      tempDutyCycle = 6;
      break;
    case 70:
      tempDutyCycle = 12;
      break;
    case 80:
      tempDutyCycle = 24;
      break;
    case 90:
      tempDutyCycle = 36;
      break;
    case 100:
      tempDutyCycle = 58;
      break;
    case 110:
      tempDutyCycle = 84;
      break;
    case 120:
      tempDutyCycle = 128;
      break;
    }

    break;

  case 'e':
    data++;
    digitalWrite(seatbelt, data[0] == '1' ? 1 : 0);
    break;

  case 'k':
    data++;
    digitalWrite(trunk, data[0] == '1' ? 1 : 0);
    break;

  case 'y':
    data++;
    digitalWrite(akumulator, data[0] == '1' ? 1 : 0);
    break;

  case 'o':
    data++; // swiatla drogowe
    digitalWrite(poduszki, data[0] == '1' ? 1 : 0);

  case 'l':

    data++;
    digitalWrite(lLights, data[0] == '1' ? 1 : 0);

    break;

  case 'h':

    data++;
    digitalWrite(kierunek, data[0] == '1' ? 1 : 0);

    break;

  case 'p':
    data++;
    digitalWrite(parking, data[0] == '1' ? 1 : 0);

    break;

  case 'a':
    data++;
    digitalWrite(ABS, data[0] == '1' ? 1 : 0);

    break;

  }
  
} 
  */