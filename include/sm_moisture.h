#include <Arduino.h>

#ifndef SM_SOILMOISTURE_H
    #define SM_SOILMOISTURE_H

    class SM_SoilMoisture {
        public:
            SM_SoilMoisture(int pin);
            int readMoistureValue();
        private:
            int _pin;
    };

#endif

