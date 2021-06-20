#include "sm_core.h"



SMNetMaster::SMNetMaster(SMNet* net) : _net(net), _ledTopic(""), _waterTopic("") { }


void SMNetMaster::setup(){

}

void SMNetMaster::handleDiscoverMessage(String &message){
    Serial.println("[Master] handleDiscoverMessage " + message);
    StaticJsonDocument<PROTOCOL_JSON_MAX_LENGHT> doc;

    deserializeJson(doc, message);

    String slaveId = doc["nodeId"];
    JsonArray modules = doc["modules"];
    String action = doc["action"];

    if (action.equals("ASK_JOIN")) {
        if (!nodeIsAlreadyJoinded(slaveId)) {
            Serial.println("[Master] Accepting node in the net: " + slaveId);
            StaticJsonDocument<1024> jsonResponse;
            jsonResponse["nodeId"] = getNodeIdentifier();
            jsonResponse["joinedId"] = slaveId;
            jsonResponse["action"] = "ACC_JOIN";
            addNodeToTheNet(slaveId);

            for(JsonVariant v : modules) {
                String module = v.as<String>();
                String topic = getNodeTopic(slaveId, module);
                jsonResponse["modules"][module] = topic;

                if (module.equals("temp")) {
                    _net->addToTopicMap(topic, [&](String &message) {
                        sm_values.temp = message.toInt();
                    });
                } else if (module.equals("hum")) {
                    _net->addToTopicMap(topic, [&](String &message) {
                        sm_values.humidity = message.toInt();
                    });
                } else if (module.equals("moisture")) {
                    _net->addToTopicMap(topic, [&](String &message) {
                        sm_values.moisture = message.toInt();
                        if (sm_values.moisture <= sm_config.moistureTreshold && sm_values.moisture != 0 ) {

                            Serial.println("[Master] Moisture level under treshold");
                            /*
                            if (_mysqlConnection.connect(addr, DB_PORT, user, pass, name)) {
                                MySQL_Cursor *cur_mem = new MySQL_Cursor(&_mysqlConnection);
                                char query[150];

                                String moisture = String(sm_values.moisture);
                                sprintf(
                                    query, 
                                    "INSERT INTO sm_alerts (value, module_id) VALUES ('%s', ( SELECT id FROM network_clients_modules WHERE topic = '%s'))", 
                                    moisture.c_str(),
                                    topic.c_str()
                                );

                                cur_mem->execute(query);
                                delete cur_mem;
                                _mysqlConnection.close();
                            } */


                            
                            if (!sm_values.nextDayRain) {
                                Serial.println("[Master] Watering plants. ");
                                _mqttClient.publish(_waterTopic, "1");
                            } else if (sm_values.moisture <= (sm_config.moistureTreshold / 2)) {
                                Serial.println("[Master] Moisture level critical, watering plants.");
                                _mqttClient.publish(_waterTopic, "1");
                            } else {
                                Serial.println("[Master] Smart raining enabled, unable to water plants.");
                            }
                        }
                    });
                } else if (module.equals("light")) {
                    _net->addToTopicMap(topic, [&](String &message) {
                        sm_values.light = message.toInt();
                    });
                } else if (module.equals("btn")) {
                    _net->addToTopicMap(topic, [&](String &message) {
                        sm_config.systemStatus = message.equals("1");
                        _mqttClient.publish(MQTT_TOPIC_STATUS, message);

                        if (sm_config.statusLed) {
                            if (!_ledTopic.equals("")) {
                                _mqttClient.publish(_ledTopic, "OFF");
                                _mqttClient.publish(_ledTopic, sm_config.systemStatus ? "GO" : "RO");
                            }
                            
                        }

                    });
                } else if (module.equals("led")) {
                    _ledTopic = topic;
                } else if (module.equals("water")) {
                    _waterTopic = topic; 
                    _net->addToTopicMap(topic, [&](String &message) {
                        sm_values.watering = message.equals("1");
                    });
                } else if (module.equals("water_lev")) {
                    _net->addToTopicMap(topic, [&](String &message) {
                        sm_values.waterTankValue = message.toInt();

                        if (sm_values.waterTankValue <= sm_config.waterTankTreshold && sm_values.waterTankValue != 0) {
                            if (_mysqlConnection.connect(addr, DB_PORT, user, pass, name)) {
                                MySQL_Cursor *cur_mem = new MySQL_Cursor(&_mysqlConnection);
                                char query[150];

                                String tmp = String(sm_values.waterTankValue);
                                sprintf(
                                    query, 
                                    "INSERT INTO sm_alerts (value, module_id) VALUES ('%s', ( SELECT id FROM network_clients_modules WHERE topic = '%s'))", 
                                    tmp.c_str(),
                                    topic.c_str()
                                );

                                cur_mem->execute(query);
                                delete cur_mem;
                                _mysqlConnection.close();
                            } 
                            
                        }
                        

                    });
                } else if (module.equals("telegram")) {
                    _net->addToTopicMap(topic, [&](String &message) {
                        if (message.equals("ON")) {
                            _mqttClient.publish(_waterTopic, "1");
                        } else if (message.equals("OFF")) {
                            _mqttClient.publish(_waterTopic, "0");
                        }
                    });
                } else if (module.equals("weather")) {
                    _net->addToTopicMap(topic, [&](String &message) {
                        if (message.toDouble() > 0.2) {
                            sm_values.nextDayRain = true;
                            Serial.println("[Master] Enabling smart rain");
                        } else {
                            sm_values.nextDayRain = false;
                        }                       
                    });
                }
            }

            String json;
            serializeJson(jsonResponse, json);
            _mqttClient.publish(MQTT_TOPIC_DISCOVER, json);


            Serial.print("[Master] publishingOnDiscover: " + slaveId);
            Serial.println(json);
        } else {
            Serial.println("[Master] Denying node in the net: " + slaveId);
            StaticJsonDocument<1024> jsonResponse;
            jsonResponse["nodeId"] = getNodeIdentifier();
            jsonResponse["joinedId"] = slaveId;
            jsonResponse["action"] = "DEN_JOIN";

            String json;
            serializeJson(jsonResponse, json);
            _mqttClient.publish(MQTT_TOPIC_DISCOVER, json);

            Serial.print("[Master] publishingOnDiscover: " + slaveId);
            Serial.println(json);
        }

        //If someone else is probclaming himself
    } else if (action.equals("PROCLAMING") && !slaveId.equals(getNodeIdentifier())) {
        ESP.reset();
    }

}

bool SMNetMaster::nodeIsAlreadyJoinded(String &nodeId) {   
    if (_mysqlConnection.connect(addr, DB_PORT, user, pass, name)) {
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&_mysqlConnection);
        Serial.println(F("[Master] Executing MySQL query nodeIsAlreadyJoinded"));

        char query[512];
        sprintf(query, "SELECT * FROM network_clients WHERE node_id = '%s' AND connected = true", nodeId.c_str());
        cur_mem->execute(query);

        column_names *cols = cur_mem->get_columns();
        row_values *row = cur_mem->get_next_row();
        _mysqlConnection.close();
        bool value = row != NULL;
        delete cur_mem;
        return value;
    } 
    return false;
}

void SMNetMaster::addNodeToTheNet(String &nodeId) {      
    if (_mysqlConnection.connect(addr, DB_PORT, user, pass, name)) {
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&_mysqlConnection);
        Serial.println(F("[Master] Executing MySQL query addNodeToTheNet"));
        char query[150];

        sprintf(
            query, 
            "INSERT INTO network_clients (node_id, connected) VALUES ('%s', 1) ON DUPLICATE KEY UPDATE connected=1, last_edit = NOW();", 
            nodeId.c_str()
        );

        cur_mem->execute(query);
        delete cur_mem;
        _mysqlConnection.close();
    } 
} 


String SMNetMaster::getNodeTopic(String &nodeId, String &module) {  
    if (_mysqlConnection.connect(addr, DB_PORT, user, pass, name)) {
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&_mysqlConnection);
        Serial.println(F("[Master] Executing MySQL query getNodeTopic"));
        String result = "";
        char query[512];
        sprintf(
            query, 
            "SELECT topic FROM network_clients_modules nmc JOIN network_clients nc ON nmc.client_id = nc.id WHERE nc.node_id = '%s' AND module = '%s'", 
            nodeId.c_str(),
            module.c_str()
            );
        cur_mem->execute(query);

        column_names *cols = cur_mem->get_columns();
        row_values *row = cur_mem->get_next_row();
        if (row != NULL) {
            result = row->values[0];
        } else {
            result =  "/go3/sensor/" + nodeId + "/module/" + module;
            sprintf(
                query, 
                "INSERT INTO network_clients_modules (module, topic, client_id) VALUES ('%s', '%s', ( SELECT id FROM network_clients WHERE node_id = '%s'))", 
                module.c_str(),
                result.c_str(),
                nodeId.c_str()
            );
            Serial.println(F("[Master] Executing MySQL query setNodeTopic"));
            cur_mem->execute(query);
        }
        delete cur_mem;
        _mysqlConnection.close();
        return result;
    } 
    return "";
}

void SMNetMaster::handleLastWillMessage(String &nodeId) {       
    if (_mysqlConnection.connect(addr, DB_PORT, user, pass, name)) {
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&_mysqlConnection);
        Serial.println(F("[Master] Executing MySQL query handleLastWillMessage"));
        char query[100];

        sprintf(
            query, 
            "UPDATE network_clients SET connected=0, last_edit = NOW() WHERE node_id = '%s';", 
            nodeId.c_str()
        );

        cur_mem->execute(query);
        delete cur_mem;
        _mysqlConnection.close();
    } 
} 
