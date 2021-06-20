#include "sm_status_btn.h"
#include "sm_core.h"

SM_StatusBtn::SM_StatusBtn(int pin) : _pin(pin), _lastBtnPressing(0) {};

bool SM_StatusBtn::isButtonPressed() {
    checkDeepSleep();
    int val = digitalRead(_pin);
    if (val == LOW) {
        if (millis() - _lastBtnPressing > 1000) {
            _lastBtnPressing = millis();
            return true;
        }
    }
    return false;
}
