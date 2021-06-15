#include <Arduino.h>

#ifndef SM_LEDSTAT_H
    #define SM_LEDSTAT_H

    class SM_StatusLed {
        public:
            SM_StatusLed(int pinR, int pinG, int pinB);
            void turnOnR();
            void turnOnG();
            void turnOnB();
            void turnOffR();
            void turnOffG();
            void turnOffB();
        private:
            int _pinR;
            int _pinG;
            int _pinB;
    };

#endif

