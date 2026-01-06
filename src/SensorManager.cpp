#include "SensorManager.h"

Adafruit_VEML7700 veml = Adafruit_VEML7700();
AHT_Sensor_Class AHT10;

SensorManager::SensorManager() {
  readings.air_qual = 0;
  readings.water_dist = 0;
  readings.water_volume = 0;
  readings.air_temp = 0;
  readings.air_qual = 0;
  readings.air_hum = 0;
  readings.soil_moist_1_raw = 0;
  readings.soil_moist_2_raw = 0;
  readings.hour = 0;
  readings.minute = 0;
  readings.second = 0;
  readings.day = 1;
  readings.month = 1;
  readings.year = 2026;
  readings.light_sensor_OK = false;
  readings.air_temp_sensor_OK = false;
  readings.soil_sensor_1_OK = false;
  readings.soil_sensor_2_OK = false;
  readings.RTC_OK = false;
  readings.water_level_sensor_OK = false;
}

bool SensorManager::init(){
  Wire.begin();
  pinMode(__TRIG_PIN, OUTPUT);
  pinMode(__ECHO_PIN, INPUT);
  pinMode(__SOIL_1_PIN, INPUT);
  pinMode(__SOIL_2_PIN, INPUT);
  All_OK = true;

  if (!init_air_qual_sensor())
  {
    readings.air_qual_sensor_OK = false;
    All_OK = false;
  } else {
    readings.air_qual_sensor_OK = true;
  }

  if (!init_air_temp_hum_sensor())
  {
    readings.air_temp_sensor_OK = false;
    All_OK = false;
  } else {
    readings.air_temp_sensor_OK = true;
  }

  if (!init_light_sensor())
  {
    readings.light_sensor_OK = false;
    All_OK = false;
  } else {
    readings.light_sensor_OK = true;
  }

  if (!init_soil_moist_sensor(__SOIL_1_PIN))
  {
    readings.soil_sensor_1_OK = false;
    All_OK = false;
  } else {
    readings.soil_sensor_1_OK = true;
  }

 if (!init_soil_moist_sensor(__SOIL_2_PIN))
  {
    readings.soil_sensor_2_OK = false;
    All_OK = false;
  } else {
    readings.soil_sensor_2_OK = true;
  }

  if (!init_RTC())
  {
    readings.RTC_OK = false;
    All_OK = false;
  } else {
    readings.RTC_OK = true;
  }

  if (!init_water_level_sensor())
  {
    readings.water_level_sensor_OK = false;
    All_OK = false;
  } else {
    readings.water_level_sensor_OK = true;
  }

  return All_OK;
}

void SensorManager::update_all() {
  readings.air_temp = readAirTemp();
  readings.air_hum = readAirHum();
  readings.air_qual = readAirQuality();
  readings.light_Lux = readLightLevel();
  readings.soil_moist_1 = readSoilMoisture(__SOIL_1_PIN);
  readings.soil_moist_2 = readSoilMoisture(__SOIL_2_PIN);
  readings.water_dist = readWaterLevel();
  readings.water_volume = readings.water_dist * 3.14159 * (SensorManager::diameter^2)/4;
  readDate();
  readTime();
}

bool init_light_sensor() {
  if (!veml.begin()) {
      return false;
  } else {
    veml.setLowThreshold(10000);
    veml.setHighThreshold(20000);
    veml.interruptEnable(false);
    return true;
  }
}

float readLightLevel() {
  return veml.readLux();
}

bool init_air_temp_hum_sensor() {
  if(!AHT10.begin(eAHT_SensorAddress_default)){
    return false;
  } else {
    return true;
  }
}

float readAirTemp() {
  return AHT10.GetTemperature();
}

float readAirHum() {
  return AHT10.GetHumidity();
}

bool init_air_qual_sensor();
bool init_soil_moist_sensor(uint8_t __SOIL_PIN);
bool init_RTC();
bool init_water_level_sensor();