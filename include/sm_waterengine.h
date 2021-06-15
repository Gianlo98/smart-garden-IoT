#include <Arduino.h>

#ifndef SM_WATERENG_H
    #define SM_WATERENG_H

    class SM_WaterEngine {
        public:
            SM_WaterEngine(int pin);
            void startWaterEngine();
            void stopWaterEngine();
            void erogateWater(int amount);
            void checkWaterEngine();
            bool newStateAvailable();
            bool readNewState();
            uint32_t getWaterTankValue();
            void setWaterTankValue(uint32_t value);

        private:
            int _pin;
            uint32_t _waterTankValue;
            unsigned long _lastWatering;
            bool _engineState;
            bool _previousEngineState; //Useful for SMNet to know if a slave should notify a state changes to master
            static const int _waterFlowRateMls; //The waterengine erogate 15 ml/s
            static const int _maxWaterDeliveryTime;
            bool isWaterEngineEnabled();
            void enableWaterEngine();
            void disableWaterEngine();
    };
#endif

