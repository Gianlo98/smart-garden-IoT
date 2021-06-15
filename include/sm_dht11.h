#include <Arduino.h>

#ifndef SM_DTH11_H    
    #define SM_DTH11_H
    #include <Adafruit_Sensor.h>
    #include <DHT.h>
    #include <DHT_U.h>
    #define DHTTYPE DHT11


    class SM_DHT11 {
        public:
            SM_DHT11(int pin);
            void printSensorDetail();
            float getTemperature();
            float getHumidity();
            void setup();
        private:
            DHT_Unified _dht;
    };

#endif