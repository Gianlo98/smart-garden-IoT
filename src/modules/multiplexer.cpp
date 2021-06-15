/*#include "sm_core.h"

const int AirValue = 700;
const int WaterValue = 300;


//Light sensor is on C0
int readLightValue() {
    digitalWrite(MUX_S0, LOW);
    delay(50);
    int val = analogRead(MUX_SIG);
    digitalWrite(MUX_S0, LOW);
    return val;
}

//Moisture sensor is on C1
int readMoistureValue() {
    digitalWrite(MUX_S0, HIGH);
    delay(50); 
    int val = map(analogRead(MUX_SIG), AirValue, WaterValue, 0, 100);
    digitalWrite(MUX_S0, LOW);
    return val;
}

*/