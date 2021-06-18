#include "sm_weather.h"
#include "sm_core.h"

const char weather_server[] = "api.openweathermap.org";
const char weather_query[] = "http://api.openweathermap.org/data/2.5/onecall?lat=%f&lon=%f&exclude=current,minutely,hourly,alert&appid=%s";


String SM_Weather::getWeatherData() {
    checkDeepSleep();

    HTTPClient http;
	String result;
	char request[500];

	sprintf(request, weather_query, WEATHER_LAT, WEATHER_LON, WEATHER_API_KEY);
	http.begin(request);
	int httpCode = http.GET();
 
    if (httpCode > 0) {
    	result = http.getString();
    }
 
    http.end();

	DynamicJsonDocument doc(6144);

	DeserializationError error = deserializeJson(doc, result);

	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		return "";
	}

	JsonArray daily = doc["daily"];
	JsonObject daily_0 = daily[0];

	/*
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
    */
	float daily_0_pop = daily_0["pop"]; // 0
    return String(daily_0_pop);
}