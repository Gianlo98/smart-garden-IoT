#include "sm_core.h"

SMCore smCore;

//SM_DHT11 m_dht(D1);
//SM_Photoresistor m_light(A0);
//SM_StatusBtn m_btn(D0);
//SM_StatusLed m_led(D2, D3, D4);
//SM_Telegram m_telegram;
SM_SoilMoisture m_moisture(A0);

void setup() {
    smCore.coreSetup();
}

void loop() {
    smCore.coreLoop();  
}


