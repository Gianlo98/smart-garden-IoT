#include "sm_core.h"

SMCore smCore;
//SM_Telegram m_telegram;
SM_Weather m_weather;
//SM_WaterEngine m_water(D5);

void setup() {
    smCore.coreSetup();
}

void loop() {
    smCore.coreLoop();  
}
