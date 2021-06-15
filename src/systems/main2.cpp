#include "sm_core.h"

SMCore smCore;


SM_SoilMoisture m_moisture(A0);

void setup() {
    smCore.coreSetup();
}

void loop() {
    smCore.coreLoop();  
}

