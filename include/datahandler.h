#include <cstdint>

typedef struct {
    int speedoDelay;
    int tachoDelay;
    int fuelAnalogVal;
} CLUSTER_DATA;

enum shiftPins {
    SPEEDO_PIN,
    TACHO_PIN,
};

extern uint8_t shift_values; // write to using bitWrite(), read from using bitRead() (from Arduino.h)

void updateSpeedoDelay(int speed);
void updateTachoDelay(int revs);
void updateFuelAnalogVal(int fuel);
void pushToShift();
void handleData();
void outputFuel();
int pulseSpeedo();
int pulseTacho();