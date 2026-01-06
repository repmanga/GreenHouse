#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VEML7700.h>

// Структура для хранения всех показаний датчиков
struct SensorReadings {
  // Датчик освещенности
  float lightLevel;     // в люксах (lux)
  
  // Датчик уровня воды (HC-SR04)
  float waterDistance;  // расстояние до воды в см
  float waterVolume;    // объем воды в мл
  
  // Датчик воздуха (AHT20/21)
  float temperature;    // температура в °C
  float humidity;       // влажность в %
  
  // Датчик качества воздуха (ENS160)
  int airQualityIndex;  // AQI (1-5)
  uint16_t tvoc;        // TVOC в ppb
  uint16_t eco2;        // eCO2 в ppm
  
  // Датчик влажности почвы (аналоговый)
  int soilMoistureRaw;  // сырое значение (0-1023)
  float soilMoisture;   // влажность почвы в %
  
  // Время (DS1307)
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  
  // Флаги состояния датчиков
  bool lightSensorOK;
  bool waterSensorOK;
  bool airSensorOK;
  bool soilSensorOK;
  bool clockOK;
};

class SensorManager {
public:
  // Конструктор
  SensorManager();
  
  // Инициализация всех датчиков
  bool begin();
  
  // Обновить все показания
  void updateAll();
  
  // Получить последние показания
  SensorReadings getReadings();
  
  // Индивидуальное чтение датчиков (для отладки)
  float readLightLevel();
  float readWaterLevel();
  void readAirData(float &temp, float &hum);
  void readAirQuality(int &aqi, uint16_t &tvoc, uint16_t &eco2);
  int readSoilMoisture();
  void readTime(uint8_t &h, uint8_t &m, uint8_t &s);
  
  // Калибровка датчика почвы
  void calibrateSoilSensor(int dryValue, int wetValue);
  
private:
  // Показания датчиков
  SensorReadings readings;
  
  // Калибровочные значения для датчика почвы
  int soilDryValue = 620;  // значение в сухой почве
  int soilWetValue = 310;  // значение в мокрой почве
  
  // Пины для датчиков
  const uint8_t SOIL_1_PIN = A0;      // Датчик влажности почвы 1
  const uint8_t SOIL_2_PIN = A1;      // Датчик влажности почвы 2
  const uint8_t TRIG_PIN = 11;      // HC-SR04 trig
  const uint8_t ECHO_PIN = 12;      // HC-SR04 echo
  
  // Приватные методы для работы с датчиками
  bool initLightSensor();
  bool initAirSensor();
  bool initClock();
  
  float readLightSensor();
  float readDistance();
  float calculateVolume(float distance);
  void readAirSensor();
  int readSoilSensor();
  void readCurrentTime();
  
  // Вспомогательные методы
  float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
};

#endif