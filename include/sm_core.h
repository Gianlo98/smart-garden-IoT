#ifndef SMCORE_H
    #define SMCORE_H
    #define SERIAL_DEBUG

    #include <ArduinoJson.h>
    #include <Arduino.h>
    #include <ESP8266WiFi.h>
    #include <ESP8266mDNS.h>
    #include <WiFiClient.h>
    #include <ESP8266WebServer.h>
    #include <InfluxDbClient.h>
    #include <uri/UriBraces.h>
    #include <MQTT.h>
    #include <MySQL_Connection.h>
    #include <MySQL_Cursor.h>
    #include "secrets.h"

    #include "sm_dht11.h"
    #include "sm_moisture.h"
    #include "sm_photoresistor.h"
    #include "sm_status_btn.h"
    #include "sm_status_led.h"
    #include "sm_waterengine.h"

    /*
    * Global variable declaration 
    */

    #ifdef M_WATER
        extern SM_WaterEngine m_water;
    #endif 

    #ifdef M_DHT
        extern SM_DHT11 m_dht;
    #endif 

    #ifdef M_MOISTURE
        extern SM_SoilMoisture m_moisture;
    #endif 

    #ifdef M_LIGHT
        extern SM_Photoresistor m_light;
    #endif 

    #ifdef M_BTN
        extern SM_StatusBtn m_btn;
    #endif 

    #ifdef M_LED
        extern SM_StatusLed m_led; 
    #endif 


    /*
    * Globally accessible struct
    */
    struct SmartGardenValues {
        float humidity, temp, light, moisture; 
        uint32_t waterTankValue;
        bool watering;   
    };

    struct SmartGardenConfig {
        uint32_t sensorUpdateInterval;     // Delay in ms of sensors updates 
        bool systemStatus;                 // Sensor reading status
        bool statusLed;                    // Status led (rgb)
        uint32_t moistureTreshold;        
        uint32_t waterTankTreshold;           
    };

    /*
    * Core classes
    */

    
    typedef std::function<void(String &payload)> SMNetHandler;
    typedef std::function<String()> SMNetSensorFunction;

    typedef struct { 
        String topic;
        SMNetHandler handler;
    } TopicMap;

    typedef struct { 
        String topic;
        SMNetSensorFunction funct;
    } SensorMap;

    typedef struct { 
        String id;
        uint32_t becomeMasterInterval;
    } MasterNode;

    typedef struct { 
        String id;
    } SlaveNode;
    
    #define PROTOCOL_JSON_MAX_LENGHT 512
    #define MQTT_TOPIC_DISCOVER "go3/discover"
    #define MQTT_TOPIC_WILL "go3/lastwill"
    #define MQTT_TOPIC_STATUS "go3/status"

    extern MQTTClient _mqttClient;
    extern WiFiClient _networkClient;
    extern WiFiClient _networkClient2;
    extern MySQL_Connection _mysqlConnection;
    extern InfluxDBClient _idbClient;
    extern ESP8266WebServer _server;

    extern IPAddress addr;
    extern char user[];   
    extern char pass[];
    extern char name[];   

    class SMNet;
    class SMNetMaster {
        public:
            SMNetMaster(SMNet* net);
            void setup();
            void handleDiscoverMessage(String &message);
            bool nodeIsAlreadyJoinded(String &nodeId);
            void addNodeToTheNet(String &nodeId);
            void handleLastWillMessage(String &message);
            void uploadDataToInflux(); 
            String _ledTopic;
            String _waterTopic;
         private: 
            SlaveNode _slaveNodes[10];
            SMNet* _net;
            uint8_t _nSlaveNodes;
            String getNodeTopic(String &nodeId, String &module);
            void checkInfluxConnection();
    };

    class SMNetSlave {
        public:
            SMNetSlave(SMNet* net);
            void publishDiscoverMessage();
            void handleDiscoverMessage(String &message);
            void handleLastWillMessage(String &message);
            bool shouldNodeBecomeMaster();
            bool isJoined;
            void addToSensorMap(String &topic, SMNetSensorFunction function, bool readOnly = true); 
            void publishSensorValues();
            void setup();
        private:
            MasterNode _masterNode;
            SMNet* _net;
            SensorMap* _readOnlySensorMap;
            SensorMap* _inputSensorMap;
            uint8_t _inputSensorMapSize;
            uint8_t _readOnlySensorMapSize;
            uint32_t _lastSensorUpdate;
            bool _readOnlyReadingsEnabled;
    };

    class SMNet {
        public:
            void handleNetwork();
            SMNet();
            void setup();
            void addToTopicMap(String topic, SMNetHandler handler);
        private:
            bool _isMasterNode;
            bool _resendDiscover;
            TopicMap* _topicMap;
            uint8_t _topicMapSize;
            SMNetSlave _netSlave; 
            SMNetMaster _netMaster;
            void updateSensorValues();
            void getDiscoverMessage();
            void handleDiscoverMessage(String &message);
            void promoteNodeToMaster();
            void checkMQTTConnection();
            void handleIncomingMessage(String &topic, String &payload);
            void startWebServer();
            void checkInfluxConnection();
            void uploadDataToInflux();
            unsigned long _lastInfluxUpdateTimeMs; 
    };

    extern String getNodeIdentifier();

    class SMCore {
        public:
            void coreSetup();
            void coreLoop();
            SMCore();
        private:
            SMNet _net;
            void checkWifiStatus();
    };

    extern void sendJSONSensorData();
    extern void sendJSONConfigData();
    extern void handleConfig();
    extern void handleWaterEngineAction();
    extern void handleSystemAction();
    extern void handleHomepage();
    extern void handleNotFound();
    extern const uint32_t _homepageHtmlLen;
    extern const uint8_t _homepageHtml[];

    extern SMCore smCore;
    extern SmartGardenValues sm_values;
    extern SmartGardenConfig sm_config; 
#endif



