#include "sm_core.h"

SMCore smCore;
SM_Telegram m_telegram;


void setup() {
    smCore.coreSetup();
}

void loop() {
    smCore.coreLoop();  
}
