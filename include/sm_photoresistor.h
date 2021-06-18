#include <Arduino.h>
#include "sm_sensor.h"

#ifndef SM_PHOTORES_H
    #define SM_PHOTORES_H

    class SM_Photoresistor : public SMSensor{
        public:
            SM_Photoresistor(int pin);
            int readLightValue();
        private:
            int _pin;
    };



#endif