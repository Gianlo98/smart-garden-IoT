#include "sm_core.h"

SMNetSlave::SMNetSlave(SMNet* net):  
isJoined(false),
 _masterNode({"", millis() + 30 * 1000}), 
 _net(net),
 _inputSensorMapSize(0),
 _readOnlySensorMapSize(0),
 _lastSensorUpdate(0),
 _readOnlyReadingsEnabled(false)  { 
    _readOnlySensorMap = new SensorMap[10];
    _inputSensorMap = new SensorMap[10];
}

/*----------------------------- DISCOVER TOPIC -------------------------------*/

void SMNetSlave::setup() {
    _net->addToTopicMap(MQTT_TOPIC_STATUS, [&](String &message) {
        _readOnlyReadingsEnabled = message.equals("1");

        if (_readOnlyReadingsEnabled) {
            Serial.println("[Slave] enabling sensor readings");
        } else {
            Serial.println("[Slave] disabling sensor readings");
        }
    });
}

void SMNetSlave::publishDiscoverMessage(){
    StaticJsonDocument<PROTOCOL_JSON_MAX_LENGHT> doc;
    doc["nodeId"] = getNodeIdentifier();
    doc["master"] = false;
    doc["action"] = "ASK_JOIN";
    JsonArray modules = doc.createNestedArray("modules");

    #ifdef M_WATER
        modules.add("water");
        modules.add("water_lev");
    #endif 

    #ifdef M_DHT
        modules.add("temp");
        modules.add("hum");
    #endif 

    #ifdef M_MOISTURE
        modules.add("moisture");    
    #endif 

    #ifdef M_LIGHT
        modules.add("light");
    #endif 

    #ifdef M_BTN
        modules.add("btn");  
    #endif 

    #ifdef M_LED
        modules.add("led");
    #endif 

    #ifdef M_TELEGRAM
        modules.add("telegram");
    #endif 

    #ifdef M_WEATHER
       modules.add("weather");
    #endif 

    String json;
    serializeJson(doc, json);
    _mqttClient.publish(MQTT_TOPIC_DISCOVER, json);

    Serial.println("[Slave] publishDiscoverMessage: " + json);
    //Master has 5s to reply
    _masterNode.becomeMasterInterval = millis() + 10 * 1000;
}

void SMNetSlave::handleDiscoverMessage(String &message) {
    Serial.println("[Slave] handleDiscoverMessage " + message);

    StaticJsonDocument<PROTOCOL_JSON_MAX_LENGHT> doc;

    deserializeJson(doc, message);

    String masterId = doc["nodeId"];
    String joinedId = doc["joinedId"];
    String action = doc["action"];

    // Master accepted me in the network
    if (getNodeIdentifier().equals(joinedId) && action.equals("ACC_JOIN")) {
        Serial.println("[Slave] Accepted inside the network ");

        JsonObject modules = doc["modules"];

        #ifdef M_DHT
            String temp_topic = modules["temp"];
            addToSensorMap(temp_topic, [&]() {
                return String(m_dht.getTemperature());
            });

            String hum_topic = modules["hum"];
            addToSensorMap(hum_topic, [&]() {
                m_dht.goToDeepSleep();
                return String(m_dht.getHumidity());
            });
        #endif 

        #ifdef M_MOISTURE
            String moisture_topic = modules["moisture"];
            if (moisture_topic != NULL) {
                addToSensorMap(moisture_topic, [&]() {
                    m_moisture.goToDeepSleep();
                    return String(m_moisture.readMoistureValue());
                });
            }
        #endif 

        #ifdef M_LIGHT
            String light_topic = modules["light"];
            addToSensorMap(light_topic, [&]() {
                m_light.goToDeepSleep();
                return String(m_light.readLightValue());
            });
        #endif 

        #ifdef M_BTN
            String btn_topic = modules["btn"];
            addToSensorMap(btn_topic, [&]() {
                if (m_btn.isButtonPressed()) {
                    m_btn.goToDeepSleep();
                    return _readOnlyReadingsEnabled ? "0" : "1";
                }
                return "";
            }, false);
        #endif 

        #ifdef M_LED
            String led_topic = modules["led"];
            _net->addToTopicMap(led_topic, [&](String &message) {
                if (message.equals("RO")) {
                    m_led.turnOnR();
                } else if (message.equals("RF")) {
                    m_led.turnOffR();
                } else if (message.equals("GO")) {
                    m_led.turnOnG();
                } else if (message.equals("GF")) {
                    m_led.turnOffG();
                } else if (message.equals("BO")) {
                    m_led.turnOnB();
                } else if (message.equals("BF")) {
                    m_led.turnOffB();
                } else if (message.equals("OFF")) {
                    m_led.turnOffR();
                    m_led.turnOffG();
                    m_led.turnOffB();
                } else if (message.equals("ON")) {
                    m_led.turnOnR();
                    m_led.turnOnG();
                    m_led.turnOnB();
                }
            });
        #endif 

        #ifdef M_WATER
            String water_topic = modules["water"];
            _net->addToTopicMap(water_topic, [&](String &message) { 

                if (message.equals("1")) {
                    m_water.startWaterEngine();
                } else if (message.equals("0")) {
                    m_water.stopWaterEngine();
                }

            });

            addToSensorMap(water_topic, [&]() -> String { 
            
                if (m_water.newStateAvailable()) {
                        return (m_water.readNewState() ? "1" : "0");
                    }
                return "";

            }, false);

            String water_tank_topic = modules["water_lev"]; 
            addToSensorMap(water_tank_topic, [&]() -> String { 
                return String(m_water.getWaterTankValue());
            });

            _net->addToTopicMap(water_tank_topic, [&](String &message) { 
                
                int newtankValue = message.toInt();
                
                if (m_water.getWaterTankValue() != newtankValue) {
                    m_water.setWaterTankValue(newtankValue);
                }

            });
        #endif 

        #ifdef M_TELEGRAM
            String telegram_topic = modules["telegram"];
            addToSensorMap(telegram_topic, [&]() {
                if (m_telegram.newMessagePresent()) {
                    String message = m_telegram.getLastMessage();
                    m_telegram.sendMessage("OK");
                    return message;
                }
                String tmp = "";
                return tmp;  
            
            }, false);
        #endif 

        #ifdef M_WEATHER
            String weather_topic = modules["weather"];
            addToSensorMap(weather_topic, [&]() {
                String weather = m_weather.getWeatherData();
                m_weather.goToDeepSleep();
                return weather;
            });
        #endif 



        
        _masterNode.id = masterId;
        isJoined = true;
    } else if (action.equals("PROCLAMING")) {
        Serial.println("[Slave] new master node " + _masterNode.id);
        _masterNode.id = masterId;
    } else if (getNodeIdentifier().equals(joinedId) && action.equals("DEN_JOIN")) { 
        ESP.reset();
    }

}

/*----------------------------- LASTWILL TOPIC -------------------------------*/

void SMNetSlave::handleLastWillMessage(String &message) {
    Serial.println("message: " + message);
    Serial.println("master: " + _masterNode.id);
    if (message.equals(_masterNode.id)) {
        Serial.println("Master goes offline");
        _masterNode.id = "";
        _masterNode.becomeMasterInterval = millis() + random(0, 3000);
    }
}

bool SMNetSlave::shouldNodeBecomeMaster() {
    #ifndef MASTER_NODE
        return false
    #else
        return _masterNode.id.equals("") && millis() > _masterNode.becomeMasterInterval; 
    #endif
}

/**
 *  Slave only function, updates all sensor values to the associated MQTT topic
*/
void SMNetSlave::addToSensorMap(String &topic, SMNetSensorFunction funct, bool readOnly) {
    Serial.println("[Slave] Registering sensor topic " + topic +  " ( " + (readOnly ? "readOnly" : "inputTopic") + ")");
    if (readOnly) {
        _readOnlySensorMap[_readOnlySensorMapSize++] = {topic, funct};
    } else {
        _inputSensorMap[_inputSensorMapSize++] = {topic, funct};
    }

}

void SMNetSlave::publishSensorValues() {
    for (int i = 0; i < _inputSensorMapSize; i++) {
        String value = _inputSensorMap[i].funct();

        //If exist a values to be updated
        if (!value.equals("")) {
            Serial.println("[Slave] publishing on topic " + _inputSensorMap[i].topic + " : " + value);
            _mqttClient.publish(_inputSensorMap[i].topic, value);
        }
    }

    if (millis() - _lastSensorUpdate > 5000 && _readOnlyReadingsEnabled) {
        Serial.println("[Slave] reading from readOnly Sensors");
        _lastSensorUpdate = millis();

        for (int i = 0; i < _readOnlySensorMapSize; i++) {
            String value = _readOnlySensorMap[i].funct();
            Serial.println("[Slave] publishing on topic " + _readOnlySensorMap[i].topic + " : " + value);
            _mqttClient.publish(_readOnlySensorMap[i].topic, value);
        }
    }
}