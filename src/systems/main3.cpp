#include "sm_core.h"

SMCore smCore;






SM_WaterEngine m_water(D7); 

void setup() {
    smCore.coreSetup();
}

void loop() {
    smCore.coreLoop();  
}

