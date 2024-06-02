#include "datahandler.h"
#include "config.h"
#include <Arduino.h>
#include <cstdint>

extern CLUSTER_DATA cluster_data;

bool impulseONS = false;
int previousMicros = 0;
int previousMicrosShift = 0;
int previousMicrosT = 0;

float SPEEDO_FREQ = 1.14f; // per km/h
float TACHO_FREQ  = 0.058f; // per 1 rev

int SHIFT_CLK = 5;
int SHIFT_DATA = 4;

int FUEL_PIN = 0;

uint8_t shift_values = 0;

void update_speed_delay(int speed) {
    float frequency = speed * SPEEDO_FREQ;
    float period = 1 / frequency;
    cluster_data.speedoDelay = period * 1000000;
}

void update_tacho_delay(int revs) {
    float frequencyT = revs * TACHO_FREQ;
    float periodT = 1 / frequencyT;

    cluster_data.tachoDelay = periodT * 1000000;
}

void updateFuelAnalogVal(int fuel) {
    cluster_data.fuelAnalogVal = map(fuel, 0, 100, 0, 255);
}

void pushToShift() {
    /*
        I admit defeat, I have zero clue why pushing out to the shift register on every loop
        causes the output to jitter - I tried decreasing the frequency with which it gets pushed out
        with no success. As such, I'll be updating the shift register on every single state change,
        which seems to not cause any issues. If i come up with a more elegant solution, I'll update
        it.
    */
    unsigned long currentMicros = micros();
    if (currentMicros - previousMicros >= 2000)
    shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, shift_values);
}

void handleData() {
    noInterrupts();
    if(pulseSpeedo() or pulseTacho()) {
        shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, shift_values); 
    } // execute only on state change
    //pulseTacho();
    outputFuel(); // doesn't utilise a shift register, so no need to shift out
    interrupts();
}

void outputFuel() {
    analogWrite(FUEL_PIN, cluster_data.fuelAnalogVal);
}

int pulseSpeedo() {
  unsigned long currentMicros = micros();
  if (currentMicros - previousMicros >= cluster_data.speedoDelay / 2)
  {
    previousMicros = currentMicros;
    bitWrite(shift_values, SPEEDO_PIN, !bitRead(shift_values, SPEEDO_PIN));
    return 1;
  }
  return 0;
}

int pulseTacho() {
    unsigned long currentMicros = micros();
    if (currentMicros - previousMicrosT >= cluster_data.tachoDelay / 2) 
    {
        previousMicrosT = currentMicros;
        bitWrite(shift_values, TACHO_PIN, !bitRead(shift_values, TACHO_PIN));
        //shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, shift_values);
        return 1; 
    }
    return 0;
}