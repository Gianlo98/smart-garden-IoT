#include <Arduino.h>

#ifndef SM_PHOTORES_H
    #define SM_PHOTORES_H

    class SM_Photoresistor {
        public:
            SM_Photoresistor(int pin);
            int readLightValue();
        private:
            int _pin;
    };



#endif