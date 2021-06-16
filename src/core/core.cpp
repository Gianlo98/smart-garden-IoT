#include "sm_core.h"

SmartGardenValues sm_values;
SmartGardenConfig sm_config; 

SMCore::SMCore() { }

void SMCore::coreSetup() {
    Serial.begin(115200);
    delay(20);


    Serial.println(F("[Core] Starting smartgarden"));
    
    _net.setup();

    // ------------- WEBSERVER INITIALIZATION ------------- //
    WiFi.mode(WIFI_STA);
}

void SMCore::coreLoop() {
    checkWifiStatus();
    _net.handleNetwork();

    #ifdef M_WATER
        m_water.checkWaterEngine();
    #endif

}

void SMCore::checkWifiStatus() {
    if (WiFi.status() != WL_CONNECTED) {
        #ifdef M_LED
            m_led.turnOffR();
            m_led.turnOffG();
            m_led.turnOffB();
        #endif
        
        Serial.print(F("[Core] Connecting to SSID: "));
        Serial.print(SECRET_SSID);
        Serial.print(F(" "));
        WiFi.begin(SECRET_SSID, SECRET_PASS);
        
        #ifdef M_LED
            bool ledBStatus = false;
        #endif
        while (WiFi.status() != WL_CONNECTED) {

            Serial.print(F("."));

            #ifdef M_LED
                if (ledBStatus) {
                    m_led.turnOnB();
                } else {
                    m_led.turnOffB();
                }
                ledBStatus = !ledBStatus;
            #endif
            delay(500);
        }
        #ifdef M_LED
            m_led.turnOffB();
        #endif

        Serial.println(F("\n[Core] Connected!"));
        
        Serial.println(F("\n=== WiFi connection status ==="));
        Serial.print(F("SSID: "));
        Serial.println(WiFi.SSID());
        Serial.print(F("Signal strength (RSSI): "));
        Serial.print(WiFi.RSSI());
        Serial.println(F(" dBm"));
        Serial.print(F("IP Address: "));
        Serial.println(WiFi.localIP());
        Serial.print(F("DNS IP: "));
        Serial.println(WiFi.dnsIP());

        Serial.println(F("==============================\n"));

    }
}
