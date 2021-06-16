#include "sm_core.h"

#define WEATHER_API_KEY "24a10b6a94da71cc9b9c3355182f9e1d"
#define WEATHER_LAT 45.513928118618466
#define WEATHER_LON 9.201990894461506
const char weather_server[] = "api.openweathermap.org";
const char weather_query[] = "http://api.openweathermap.org/data/2.5/onecall?lat=%f&lon=%f&exclude=current,minutely,hourly,alert&appid=%s";


WiFiClient client;

void setup() {
	WiFi.mode(WIFI_STA);

	Serial.begin(115200);
	Serial.println(F("\n\nSetup completed.\n\n"));
}



void fetchWeatherData() {

	HTTPClient http;
	String result;
	char request[500];
	sprintf(request, weather_query, WEATHER_LAT, WEATHER_LON, WEATHER_API_KEY);
	http.begin(request);
	Serial.println(request);
	int httpCode = http.GET();
 
    if (httpCode > 0) {
    	result = http.getString();
      	Serial.println(result);
    }
 
    http.end();
	Serial.println(httpCode);

	DynamicJsonDocument doc(6144);

	DeserializationError error = deserializeJson(doc, result);

	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		return;
	}

	float lat = doc["lat"]; // 45.5139
	float lon = doc["lon"]; // 9.202
	const char* timezone = doc["timezone"]; // "Europe/Rome"
	int timezone_offset = doc["timezone_offset"]; // 7200

	JsonArray daily = doc["daily"];

	JsonObject daily_0 = daily[0];
	long daily_0_dt = daily_0["dt"]; // 1623754800

	JsonObject daily_0_temp = daily_0["temp"];
	float daily_0_temp_day = daily_0_temp["day"]; // 300.12
	float daily_0_temp_min = daily_0_temp["min"]; // 293.8
	float daily_0_temp_max = daily_0_temp["max"]; // 301.99
	float daily_0_temp_night = daily_0_temp["night"]; // 296.99
	float daily_0_temp_eve = daily_0_temp["eve"]; // 300.27
	float daily_0_temp_morn = daily_0_temp["morn"]; // 293.83

	int daily_0_pressure = daily_0["pressure"]; // 1019
	int daily_0_humidity = daily_0["humidity"]; // 37


	int daily_0_pop = daily_0["pop"]; // 0

	Serial.println(daily_0_pop);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print(F("Connecting to SSID: "));
    Serial.println(SECRET_SSID);
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(F("."));
      delay(250);
    }
    Serial.println(F("\nConnected!"));
  }

  fetchWeatherData();
  delay(60000);
}