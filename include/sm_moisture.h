#include <Arduino.h>
#include "sm_sensor.h"

#ifndef SM_SOILMOISTURE_H
    #define SM_SOILMOISTURE_H

    class SM_SoilMoisture : public SMSensor{
        public:
            SM_SoilMoisture(int pin);
            int readMoistureValue();
        private:
            int _pin;
    };

#endif

