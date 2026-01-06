#include "SensorManager.h"

#define ENS160_ADDRESS 0x52

Adafruit_VEML7700 veml = Adafruit_VEML7700();
AHT_Sensor_Class AHT10;
ENS160 ens160;
tmElements_t tm;

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

SensorManager::SensorManager() {
  readings.air_qual = 0;
  readings.water_dist = 0;
  readings.water_volume = 0;
  readings.air_temp = 0;
  readings.air_qual = 0;
  readings.air_hum = 0;
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
  diameter = 100;
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

  delay(10);

  if (!init_air_temp_hum_sensor())
  {
    readings.air_temp_sensor_OK = false;
    All_OK = false;
  } else {
    readings.air_temp_sensor_OK = true;
  }

  delay(10);

  if (!init_light_sensor())
  {
    readings.light_sensor_OK = false;
    All_OK = false;
  } else {
    readings.light_sensor_OK = true;
  }

  delay(10);

  if (!init_soil_moist_sensor(__SOIL_1_PIN))
  {
    readings.soil_sensor_1_OK = false;
    All_OK = false;
  } else {
    readings.soil_sensor_1_OK = true;
  }

  delay(10);
  
 if (!init_soil_moist_sensor(__SOIL_2_PIN))
  {
    readings.soil_sensor_2_OK = false;
    All_OK = false;
  } else {
    readings.soil_sensor_2_OK = true;
  }

  delay(10);
  
  if (!init_RTC())
  {
    readings.RTC_OK = false;
    All_OK = false;
  } else {
    readings.RTC_OK = true;
  }

  delay(10);
  
  if (!init_water_level_sensor())
  {
    readings.water_level_sensor_OK = false;
    All_OK = false;
  } else {
    readings.water_level_sensor_OK = true;
  }

  delay(10);
  
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
  readings.water_volume = readings.water_dist * 3.14159 * (diameter^2)/4;
  RTC.read(tm);
  readings.second = tm.Second;
  readings.minute = tm.Minute;
  readings.hour = tm.Hour;
  readings.day = tm.Day;
  readings.month = tm.Month;
  readings.year = tm.Year;
}

bool init_light_sensor() {
  if (!veml.begin()) {
      return false;
  } else {
    veml.setLowThreshold(light_low_threshold);
    veml.setHighThreshold(light_high_threshold);
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

bool init_air_qual_sensor() {
  ens160.begin(&Wire, ENS160_ADDRESS);
  if(!ens160.init()){
    return false;
  } else {
    return true;
  }
}

float readAirQuality() {
  return ens160.getAirQualityIndex_UBA(); // TODO: test this method
}

bool init_soil_moist_sensor(uint8_t __SOIL_PIN) {
  if (!(analogRead(__SOIL_PIN) > 0 || analogRead(__SOIL_PIN) < 1024))
  {
    return false;
  } else {
    return true;
  }
}

bool init_soil_moist_sensor(uint8_t __SOIL_PIN) {
  return map(analogRead(__SOIL_PIN), 0, 1024, 0, 100);
}

bool init_RTC() {
  bool parse=false;
  bool config=false;
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }
  delay(200);
  return true;
}

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

bool init_water_level_sensor();