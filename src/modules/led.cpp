#include "sm_status_led.h"
#include "sm_core.h"

SM_StatusLed::SM_StatusLed(int pinR, int pinG, int pinB) : _pinR(pinR), _pinG(pinG), _pinB(pinB) { 
    pinMode(_pinR, OUTPUT);
    pinMode(_pinG, OUTPUT);
    pinMode(_pinB, OUTPUT);

}

void SM_StatusLed::turnOnR() {
    digitalWrite(_pinR, HIGH);
}
void SM_StatusLed::turnOnG() {
    digitalWrite(_pinG, HIGH);
}
void SM_StatusLed::turnOnB() {
    digitalWrite(_pinB, HIGH);
}
void SM_StatusLed::turnOffR() {
    digitalWrite(_pinR, LOW);
}
void SM_StatusLed::turnOffG() {
    digitalWrite(_pinG, LOW);
}
void SM_StatusLed::turnOffB() {
    digitalWrite(_pinB, LOW);
}



