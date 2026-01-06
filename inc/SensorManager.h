#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VEML7700.h>
#include <Thinary_AHT_Sensor.h>
#include <DS1307RTC.h>
#include <ens160.h>
#include <HCSR04.h>
#include <time.h>

// Цель -- создать класс для управления всеми сенсорами

struct SensorReadings {

  float light_Lux;

  float air_temp;

  float air_qual; // TODO: посмотреть, можно ли избавиться от float
  uint8_t air_hum;

  float water_dist;
  float water_volume;

  uint8_t soil_moist_1_raw;
  uint8_t soil_moist_2_raw;
  uint8_t soil_moist_1;
  uint8_t soil_moist_2;

  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t day;
  uint8_t month;
  uint8_t year;

  bool light_sensor_OK;
  bool air_temp_sensor_OK;
  bool air_qual_sensor_OK;
  bool soil_sensor_1_OK;
  bool soil_sensor_2_OK;
  bool RTC_OK;
  bool water_level_sensor_OK;
};

class SensorManager
{
private:

  SensorReadings readings;
  uint8_t __soil_Dry_val = 620;
  uint8_t __soil_Wet_val = 310;

  const uint16_t diameter, pump_flow = 100;
  const uint8_t __SOIL_1_PIN = A0;
  const uint8_t __SOIL_2_PIN = A1;
  const uint8_t __TRIG_PIN = 11;
  const uint8_t __ECHO_PIN = 12;
  bool All_OK;

  bool init_light_sensor();
  bool init_air_qual_sensor();
  bool init_air_temp_hum_sensor();
  bool init_soil_moist_sensor(uint8_t __SOIL_PIN);
  bool init_RTC();
  bool init_water_level_sensor();

public:

  SensorManager();
  
  bool init();
  void update_all();

  SensorReadings getReadings();

  bool is_Light_Sensor_OK() { return readings.light_sensor_OK; };
  bool is_Water_Sensor_OK() { return readings.water_level_sensor_OK; };
  bool is_Air_temp_hum_Sensor_OK() { return readings.air_temp_sensor_OK; };
  bool is_Soil_Sensor_1_OK() { return readings.soil_sensor_1_OK; };
  bool is_Soil_Sensor_2_OK() { return readings.soil_sensor_2_OK; };
  bool is_RTC_OK() { return readings.RTC_OK; };
  
  float readLightLevel();
  float readWaterLevel();
  float readAirTemp();
  float readAirHum();
  float readAirQuality();
  uint8_t readSoilMoisture(uint8_t __SOIL_PIN);
  void readTime();
  void readDate();
};

#endif