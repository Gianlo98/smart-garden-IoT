#include "sm_core.h"

MQTTClient _mqttClient(2048);
WiFiClient _networkClient;
WiFiClient _networkClient2;
MySQL_Connection _mysqlConnection((Client *)&_networkClient2);
ESP8266WebServer _server(80);

#ifdef M_INFLUX
InfluxDBClient _idbClient(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);
#endif

IPAddress addr(DB_IP); 
char user[] = DB_USER;   
char pass[] = DB_PASS;
char name[] = DB_NAME;  

//Constructor
SMNet::SMNet() : 
_isMasterNode(false), 
_resendDiscover(false),
_topicMapSize(0),
_netSlave(this), 
_netMaster(this) {
	_topicMap = new TopicMap[10];
	addToTopicMap(MQTT_TOPIC_DISCOVER, [&](String &message) {
		if (_isMasterNode) {
			_netMaster.handleDiscoverMessage(message);
		}

		_netSlave.handleDiscoverMessage(message);
	});

	addToTopicMap(MQTT_TOPIC_WILL, [&](String &message) {
		_netSlave.handleLastWillMessage(message);
		 if (_isMasterNode) {
			_netMaster.handleLastWillMessage(message);
		}
	});
		
}

void SMNet::addToTopicMap(String topic, SMNetHandler handler) {
	Serial.println("[SMNet] Adding to topicMap " + topic);
	_mqttClient.subscribe(topic);
	_topicMap[_topicMapSize++] = {topic, handler};
}


//Setup method
void SMNet::setup() {
	_mqttClient.setWill(MQTT_TOPIC_WILL, getNodeIdentifier().c_str());
	_mqttClient.begin(MQTT_BROKERIP, MQTT_PORT, _networkClient);
	_mqttClient.onMessage([this](String &topic, String &payload) {
		handleIncomingMessage(topic, payload);
	}); 
	_netSlave.setup();
}

void SMNet::checkMQTTConnection() {
	
	if (!_mqttClient.connected()) {   // not connected
		Serial.println("[SMNet] Broker is disconnected");
		Serial.println(_mqttClient.lastError());
		Serial.println(_mqttClient.returnCode());
		Serial.println(F("[SMNet] Connecting to MQTT broker"));
		while (!_mqttClient.connect(getNodeIdentifier().c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
			Serial.print(".");
			delay(1000);
		}
		Serial.println(F("[SMNet] Connected!"));
		for (int i = 0; i < _topicMapSize; i++) {
			 Serial.println("[SMNet] Subscribed to topic" + _topicMap[i].topic);
			_mqttClient.subscribe(_topicMap[i].topic);
		}
		_netSlave.isJoined = false;
		_resendDiscover = true;
	}
}


void SMNet::handleNetwork() {
	if (_isMasterNode) {
		MDNS.update();
		_server.handleClient(); 

		if (millis() - _lastInfluxUpdateTimeMs > sm_config.sensorUpdateInterval && sm_config.systemStatus) {
			_lastInfluxUpdateTimeMs = millis();
			uploadDataToInflux();
		}
	}
	
	_mqttClient.loop();  
	checkMQTTConnection();

	if (_netSlave.shouldNodeBecomeMaster() && !_isMasterNode) {
		promoteNodeToMaster();
	}

	if (_resendDiscover) {
		_resendDiscover = false;
		_netSlave.publishDiscoverMessage();
	}

	_netSlave.publishSensorValues();
}

void SMNet::handleIncomingMessage(String &topic, String &payload) {
	
	Serial.println("[SMNet] Recived message on topic" + topic);
	for (int i = 0; i < _topicMapSize; i++) {
		if (_topicMap[i].topic.equals(topic)) {
			_topicMap[i].handler(payload);
		}
	}
	
}

void SMNet::promoteNodeToMaster() {
	StaticJsonDocument<PROTOCOL_JSON_MAX_LENGHT> doc;
    #ifndef MASTER_NODE
        ESP.reset();
    #endif

	doc["nodeId"] = getNodeIdentifier();
	doc["master"] = true;
	doc["action"] = "PROCLAMING";
	String json;
	serializeJson(doc, json);
	_mqttClient.publish(MQTT_TOPIC_DISCOVER, json);
	Serial.println("[SMNet] promoting node to master: " + json);
	_isMasterNode = true;
	_resendDiscover = true;
 
	if (_mysqlConnection.connect(addr, DB_PORT, user, pass, name)) {
		MySQL_Cursor *cur_mem = new MySQL_Cursor(&_mysqlConnection);
		//Master is the first connected client and
		cur_mem->execute("UPDATE network_clients SET connected=0, last_edit = NOW() WHERE connected = 1;");
		cur_mem->execute("SELECT * FROM sm_config");
		column_names *cols = cur_mem->get_columns();
		row_values *row = NULL;
		do {
			row = cur_mem->get_next_row();
			if (row != NULL) {
				String confName = row->values[1];
				String confValue = row->values[2];

				if (confName.equals("sensorUpdateInterval")) {

					sm_config.sensorUpdateInterval = confValue.toInt();
					Serial.println("[SMNet] Config detected sensorUpdateInterval:" + String(sm_config.sensorUpdateInterval));
				} else if (confName.equals("systemStatus")) {

					 sm_config.systemStatus = confValue.equals("1");
					 Serial.println("[SMNet] Config detected systemStatus:" +  String(sm_config.systemStatus));
				} else if (confName.equals("statusLed")) {

					sm_config.statusLed = confValue.equals("1");
					Serial.println("[SMNet] Config detected statusLed:" +  String(sm_config.statusLed));
				} else if (confName.equals("moistureTreshold")) {

					sm_config.moistureTreshold = confValue.toInt();
					Serial.println("[SMNet] Config detected moistureTreshold:" +  String(sm_config.moistureTreshold));
				} else if (confName.equals("waterTankTreshold")) {

					sm_config.waterTankTreshold = confValue.toInt();
					Serial.println("[SMNet] Config detected waterTankTreshold:" +  String(sm_config.waterTankTreshold));
				}
			}
		} while (row != NULL);

		_mysqlConnection.close();
		delete cur_mem;
	} 

	startWebServer();
	if (MDNS.begin("smartgarden")) {
		Serial.println("[SMNet] MDNS responder started");
	}

	checkInfluxConnection();


}

void SMNet::startWebServer() {
   _server.on("/", HTTP_GET, [this]{handleHomepage();});
	_server.on("/config", HTTP_POST,  [this]{handleConfig();});
	_server.on("/sensor", HTTP_GET,  [this]{sendJSONSensorData();});
	_server.on("/config", HTTP_GET,  [this]{sendJSONConfigData();});
	_server.on(UriBraces("/action/we/{}"), HTTP_GET, [this]{
		String action = _server.pathArg(0);
	
		if (action == "on") {
			_mqttClient.publish(_netMaster._waterTopic, "1");
		} else if (action == "off") {
		   _mqttClient.publish(_netMaster._waterTopic, "0");
		} else {
			_server.send(404, "text/plain", "Action not found");
		}

		_server.send(200, "text/plain", "done");
	});
	_server.on(UriBraces("/action/sys"), HTTP_GET, [this]{handleSystemAction();});
	_server.begin();

	Serial.println("[SMNet] HTTP server started");

}

void SMNet::checkInfluxConnection() {
	#ifdef M_INFLUX
	if (_idbClient.validateConnection()) {
		Serial.print(F("[SMNet] Connected to InfluxDB: "));
		Serial.println(_idbClient.getServerUrl());
	} else {
		Serial.print(F("[SMNet] InfluxDB connection failed: "));
		Serial.println(_idbClient.getLastErrorMessage());
	}
	#endif
}



String getNodeIdentifier() {
	return WiFi.macAddress();
}

void SMNet::uploadDataToInflux() {
	#ifdef M_INFLUX

	Point pointDevice("smartgarden_sensor");

	pointDevice.addTag("device", "ESP8266");
	pointDevice.addTag("ChipId", String(ESP.getChipId()));

	pointDevice.addField("humidity", sm_values.humidity);
	pointDevice.addField("temp", sm_values.temp);
	pointDevice.addField("light", sm_values.light);
	pointDevice.addField("moisture", sm_values.moisture);
	pointDevice.addField("waterTankValue", sm_values.waterTankValue);
	pointDevice.addField("watering", sm_values.watering);

	if (_idbClient.writePoint(pointDevice)) { 
		
		Serial.print(F("[SMNet] Data uploaded to influx"));
		Serial.println("");

	} else {

		Serial.println(F("[SMNet] InfluxDB write failed: "));
		Serial.println(_idbClient.getLastErrorMessage());

	}
	#endif

}
