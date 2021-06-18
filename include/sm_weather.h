#include <Arduino.h>
#include <ESP8266TelegramBOT.h>
#include "sm_sensor.h"

#ifndef SM_WEATHER_H
    #define SM_WEATHER_H

    class SM_Weather : public SMSensor {
        public:
            String getWeatherData();
    };

#endif

