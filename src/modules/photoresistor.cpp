#include "sm_photoresistor.h"


SM_Photoresistor::SM_Photoresistor(int pin) {
    pinMode(pin, OUTPUT);
    _pin = pin;
}

int SM_Photoresistor::readLightValue() {
    return analogRead(_pin);
}