#include <Arduino.h>

#ifndef SM_SENSOR_H   
    #define SM_SENSOR_H

    class SMSensor {
        public:
            void goToDeepSleep();
            bool _shoudGoToDeepSleep;
            void checkDeepSleep();
    };
    
#endif