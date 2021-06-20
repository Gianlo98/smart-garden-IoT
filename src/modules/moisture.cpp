#include "sm_moisture.h"

const int AirValue = 700;
const int WaterValue = 300;
//Moisture sensor is on C1

SM_SoilMoisture::SM_SoilMoisture(int pin) {
    _pin = pin;
}

int SM_SoilMoisture::readMoistureValue() {
    checkDeepSleep();
    return map(analogRead(_pin), AirValue, WaterValue, 0, 100);
}