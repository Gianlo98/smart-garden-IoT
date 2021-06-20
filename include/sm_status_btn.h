#include <Arduino.h>
#include "sm_sensor.h"

#ifndef SM_STATUSBTN_H
    #define SM_STATUSBTN_H

    class SM_StatusBtn : public SMSensor{
        public:
            SM_StatusBtn(int pin);
            bool isButtonPressed();
        private:
            int _pin;
            int _lastBtnPressing;
    };


#endif

