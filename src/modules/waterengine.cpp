#include "sm_core.h"
#include "sm_waterengine.h"
#define MQTT_TOPIC_STA "go3/sensors/water/status"

const int SM_WaterEngine::_waterFlowRateMls = 15; //The waterengine erogates 15 ml/s
const int SM_WaterEngine::_maxWaterDeliveryTime = 20 * 1000;


SM_WaterEngine::SM_WaterEngine(int pin) : _pin(pin), _waterTankValue(2000) {
    pinMode(pin, OUTPUT);
    disableWaterEngine();
}

void SM_WaterEngine::startWaterEngine() {
    if (!isWaterEngineEnabled()) {
        _lastWatering = millis();
        enableWaterEngine();

        //config.statusLed = false;
        //updateLed();


        Serial.println(F("[SM_WaterEngine] Starting waterengine"));

        
    }
}

void SM_WaterEngine::stopWaterEngine() {
    if (isWaterEngineEnabled()) {
        if (_lastWatering < millis()) {
            long timeDifference = millis() - _lastWatering;
            uint32_t waterDelivered = (timeDifference / 1000) * _waterFlowRateMls;

            disableWaterEngine();
            _lastWatering = millis();
            _waterTankValue -= waterDelivered;

            //config.statusLed = true;
            //updateLed();

            #ifdef SERIAL_DEBUG
                Serial.print(F("[SM_WaterEngine] Stopping waterengine. (active for: "));
                Serial.print(timeDifference);
                Serial.print(F("ms, water delivered: "));
                Serial.print(waterDelivered);
                Serial.println(F("ml)"));
            #endif
        }
    }
}

void SM_WaterEngine::erogateWater(const int amountMl) {
    long time = (amountMl / _waterFlowRateMls) * 1000;
    time = time < _maxWaterDeliveryTime ? time : _maxWaterDeliveryTime;  
    startWaterEngine();
    delay(time);
    stopWaterEngine();
}   

void SM_WaterEngine::checkWaterEngine() {
    if (isWaterEngineEnabled()) {
        if (millis() - _lastWatering >= _maxWaterDeliveryTime) {
            #ifdef SERIAL_DEBUG
                Serial.print(F("[SM_WaterEngine] Max water delivery time reached. (20s)"));
                Serial.println("");
            #endif
            stopWaterEngine();
        }
    }
}

bool SM_WaterEngine::isWaterEngineEnabled() {
    return _engineState;
}

void SM_WaterEngine::enableWaterEngine() {
    digitalWrite(_pin, LOW);
    _engineState = true;
}


void SM_WaterEngine::disableWaterEngine() {
    digitalWrite(_pin, HIGH);
    _engineState = false;
}

bool SM_WaterEngine::newStateAvailable() {
    return _engineState != _previousEngineState;
}

bool SM_WaterEngine::readNewState() {
    _previousEngineState = _engineState;
    return _engineState;  
}

uint32_t SM_WaterEngine::getWaterTankValue() {
    return _waterTankValue;
}

void SM_WaterEngine::setWaterTankValue(uint32_t value) {
    _waterTankValue = value;
}