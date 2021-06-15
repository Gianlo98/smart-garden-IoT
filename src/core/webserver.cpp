#include "sm_core.h"

void sendJSONSensorData() {
    StaticJsonDocument<1024> jsonResponse;
    jsonResponse["humidity"] = sm_values.humidity;
    jsonResponse["temp"] = sm_values.temp;
    jsonResponse["light"] = sm_values.light;
    jsonResponse["moisture"] = sm_values.moisture;
    jsonResponse["waterTankValue"] = sm_values.waterTankValue;
    jsonResponse["watering"] = sm_values.watering;

    String json;
    serializeJson(jsonResponse, json);

    _server.send(200, "text/json", json);
}

void sendJSONConfigData() {
   StaticJsonDocument<1024> jsonResponse;
    jsonResponse["sensorUpdateInterval"] = sm_config.sensorUpdateInterval;
    jsonResponse["systemStatus"] = sm_config.systemStatus;
    jsonResponse["statusLed"] = sm_config.statusLed;
    jsonResponse["moistureTreshold"] = sm_config.moistureTreshold;
    jsonResponse["waterTankTreshold"] = sm_config.waterTankTreshold;

    String json;
    serializeJson(jsonResponse, json);


    _server.send(200, F("text/json"), json);
}


void handleConfig() {
    if (_server.hasArg("plain") == false) {
        _server.send(200, "text/plain", "Body not received");
        return;
    }

    StaticJsonDocument<1024> jsonRequest;
 
    DeserializationError error = deserializeJson(jsonRequest, _server.arg("plain"));

    if (error) {
        _server.send(500, "text/plain", "Invalid JSON");
        return;
    }

    if (jsonRequest.containsKey("sensorUpdateInterval")) {
        sm_config.sensorUpdateInterval = jsonRequest["sensorUpdateInterval"];

        if (_mysqlConnection.connect(addr, DB_PORT, user, pass, name)) {
            MySQL_Cursor *cur_mem = new MySQL_Cursor(&_mysqlConnection);
            char query[150];
            
            String tmp = String(sm_config.sensorUpdateInterval);
            sprintf(
                query, 
                "UPDATE sm_config sc SET value = '%s' WHERE sc.key = '%s'", 
                tmp.c_str(),
                "sensorUpdateInterval"
            );

            cur_mem->execute(query);
            delete cur_mem;
            _mysqlConnection.close();
        } 
    }

    if (jsonRequest.containsKey("moistureTreshold")) {
        sm_config.moistureTreshold = jsonRequest["moistureTreshold"];

        if (_mysqlConnection.connect(addr, DB_PORT, user, pass, name)) {
            MySQL_Cursor *cur_mem = new MySQL_Cursor(&_mysqlConnection);
            char query[150];

            sprintf(
                query, 
                "UPDATE sm_config sc SET value = '%s' WHERE sc.key = '%s'", 
                String(sm_config.moistureTreshold).c_str(),
                "moistureTreshold"
            );

            cur_mem->execute(query);
            delete cur_mem;
            _mysqlConnection.close();
        } 
    }
    if (jsonRequest.containsKey("waterTankTreshold")) {
        sm_config.waterTankTreshold = jsonRequest["waterTankTreshold"];

        if (_mysqlConnection.connect(addr, DB_PORT, user, pass, name)) {
            MySQL_Cursor *cur_mem = new MySQL_Cursor(&_mysqlConnection);
            char query[150];

            sprintf(
                query, 
                "UPDATE sm_config sc SET value = '%s' WHERE sc.key = '%s'", 
                String(sm_config.waterTankTreshold).c_str(),
                "waterTankTreshold"
            );

            cur_mem->execute(query);
            delete cur_mem;
            _mysqlConnection.close();
        } 
    }
     
    if (jsonRequest.containsKey("systemStatus")) {
        sm_config.systemStatus = jsonRequest["systemStatus"] == "1";

        if (_mysqlConnection.connect(addr, DB_PORT, user, pass, name)) {
            MySQL_Cursor *cur_mem = new MySQL_Cursor(&_mysqlConnection);
            char query[150];

            sprintf(
                query, 
                "UPDATE sm_config sc SET value = '%s' WHERE sc.key = '%s'", 
                sm_config.systemStatus ? "1" : 0,
                "systemStatus"
            );

            cur_mem->execute(query);
            delete cur_mem;
            _mysqlConnection.close();
        } 
    }

    if (jsonRequest.containsKey("statusLed")) {
        sm_config.statusLed = jsonRequest["statusLed"] == "1";

        if (_mysqlConnection.connect(addr, DB_PORT, user, pass, name)) {
            MySQL_Cursor *cur_mem = new MySQL_Cursor(&_mysqlConnection);
            char query[150];

            sprintf(
                query, 
                "UPDATE sm_config sc SET value = '%s' WHERE sc.key = '%s'", 
                sm_config.statusLed ? "1" : 0,
                "statusLed"
            );

            cur_mem->execute(query);
            delete cur_mem;
            _mysqlConnection.close();
        } 
    }

    

    sendJSONConfigData();
}



void handleSystemAction() {
    ESP.reset();
}

void handleHomepage() {
    _server.send(200, "text/html", _homepageHtml, _homepageHtmlLen);
}

void handleNotFound() {
  _server.send(404, F("text/plain"), F("Not found"));
}
