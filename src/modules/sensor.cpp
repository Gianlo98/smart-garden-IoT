#include "sm_sensor.h"
#include "sm_core.h"

void SMSensor::goToDeepSleep() {
    #ifndef MASTER_NODE
        #ifdef DEEPSLEEP_TIME
        _shoudGoToDeepSleep = true;
        #endif
    #endif
}

void SMSensor::checkDeepSleep() {
    if (_shoudGoToDeepSleep) {
        #ifdef DEEPSLEEP_TIME
            Serial.println("[Sensor] Going into deep sleep mode");
            ESP.deepSleep(DEEPSLEEP_TIME);
        #endif
    }
}

