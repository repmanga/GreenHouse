#include "SensorManager.h"
#include <Wire.h>

// Заглушки для библиотек - мы их добавим позже
// Пока будем использовать симуляцию для тестирования

SensorManager::SensorManager() {
  // Инициализируем значения по умолчанию
  readings.lightLevel = 0;
  readings.waterDistance = 0;
  readings.waterVolume = 0;
  readings.temperature = 20.0;
  readings.humidity = 50.0;
  readings.airQualityIndex = 1;
  readings.tvoc = 0;
  readings.eco2 = 400;
  readings.soilMoistureRaw = 0;
  readings.soilMoisture = 0;
  readings.hour = 0;
  readings.minute = 0;
  readings.second = 0;
  
  // Все датчики изначально "не в порядке"
  readings.lightSensorOK = false;
  readings.waterSensorOK = false;
  readings.airSensorOK = false;
  readings.soilSensorOK = false;
  readings.clockOK = false;
}

bool SensorManager::begin() {
  Serial.println(F("Инициализация датчиков..."));
  
  // Начинаем Wire для I2C
  Wire.begin();
  
  // Настраиваем пины для HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Настраиваем пин для датчика почвы
  pinMode(SOIL_PIN, INPUT);
  
  // Инициализация I2C датчиков
  bool allOK = true;
  
  // 1. Датчик освещенности
  if (!initLightSensor()) {
    Serial.println(F("Ошибка: датчик освещенности не найден"));
    readings.lightSensorOK = false;
    allOK = false;
  } else {
    readings.lightSensorOK = true;
    Serial.println(F("Датчик освещенности OK"));
  }
  
  // 2. Датчик воздуха (температура/влажность)
  if (!initAirSensor()) {
    Serial.println(F("Ошибка: датчик воздуха не найден"));
    readings.airSensorOK = false;
    allOK = false;
  } else {
    readings.airSensorOK = true;
    Serial.println(F("Датчик воздуха OK"));
  }
  
  // 3. Часы реального времени
  if (!initClock()) {
    Serial.println(F("Ошибка: часы RTC не найдены"));
    readings.clockOK = false;
    allOK = false;
  } else {
    readings.clockOK = true;
    Serial.println(F("Часы RTC OK"));
  }
  
  // 4. Датчики с цифровыми/аналоговыми пинами всегда "OK"
  readings.waterSensorOK = true;
  readings.soilSensorOK = true;
  
  Serial.println(F("Инициализация датчиков завершена"));
  return allOK;
}

void SensorManager::updateAll() {
  // Обновляем все показания
  readings.lightLevel = readLightSensor();
  readings.waterDistance = readDistance();
  readings.waterVolume = calculateVolume(readings.waterDistance);
  readAirSensor();  // обновляет temperature, humidity, AQI, TVOC, eCO2
  readings.soilMoistureRaw = readSoilSensor();
  readings.soilMoisture = mapFloat(
    readings.soilMoistureRaw, 
    soilWetValue, 
    soilDryValue, 
    100.0, 
    0.0
  );
  readCurrentTime();  // обновляет hour, minute, second
}

SensorReadings SensorManager::getReadings() {
  return readings;
}

// === Реализация работы с датчиками ===

bool SensorManager::initLightSensor() {
  // Заглушка для VEML7700
  // В реальности здесь будет код инициализации
  // return lightSensor.begin();
  
  // Для тестирования всегда возвращаем true
  delay(10);
  return true;
}

bool SensorManager::initAirSensor() {
  // Заглушка для AHT20 и ENS160
  // В реальности здесь будет код инициализации обоих датчиков
  
  delay(10);
  return true;
}

bool SensorManager::initClock() {
  // Заглушка для DS1307
  delay(10);
  return true;
}

float SensorManager::readLightSensor() {
  // Заглушка для VEML7700
  // В реальности: return lightSensor.readLux();
  
  // Симуляция: днем 500-1000 люкс, ночью 0-50
  uint8_t h = readings.hour;
  if (h >= 6 && h <= 18) {
    return random(500, 1000);  // "дневной" свет
  } else {
    return random(0, 50);      // "ночной" свет
  }
}

float SensorManager::readDistance() {
  // Измерение расстояния с HC-SR04
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Ждем отклик (таймаут 30000 мкс = 30 мс)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  
  if (duration == 0) {
    // Ошибка измерения
    readings.waterSensorOK = false;
    return 0;
  }
  
  readings.waterSensorOK = true;
  // Расстояние = (время * скорость звука) / 2
  // Скорость звука ~ 0.0343 см/мкс
  float distance = duration * 0.0343 / 2;
  
  return distance;
}

float SensorManager::calculateVolume(float distance) {
  // Предположим цилиндрический бак:
  // - Высота: 30 см
  // - Диаметр: 20 см (радиус 10 см)
  
  const float TANK_HEIGHT = 30.0;   // см
  const float TANK_RADIUS = 10.0;   // см
  
  // Высота воды от дна
  float waterHeight = TANK_HEIGHT - distance;
  
  // Если расстояние больше высоты бака или отрицательное
  if (waterHeight < 0) waterHeight = 0;
  if (waterHeight > TANK_HEIGHT) waterHeight = TANK_HEIGHT;
  
  // Объем цилиндра: V = π * r² * h
  // 1 см³ = 1 мл
  float volume = PI * TANK_RADIUS * TANK_RADIUS * waterHeight;
  
  return volume;
}

void SensorManager::readAirSensor() {
  // Заглушка для AHT20 (температура/влажность)
  // В реальности:
  // readings.temperature = aht.readTemperature();
  // readings.humidity = aht.readHumidity();
  
  // Симуляция реалистичных значений
  readings.temperature = 20.0 + random(-50, 50) * 0.1;  // 15-25°C
  readings.humidity = 50.0 + random(-30, 30) * 0.1;     // 20-80%
  
  // Заглушка для ENS160 (качество воздуха)
  // В реальности:
  // readings.airQualityIndex = ens160.getAQI();
  // readings.tvoc = ens160.getTVOC();
  // readings.eco2 = ens160.geteCO2();
  
  readings.airQualityIndex = random(1, 4);  // 1-3
  readings.tvoc = random(0, 500);           // 0-500 ppb
  readings.eco2 = 400 + random(0, 600);     // 400-1000 ppm
}

int SensorManager::readSoilSensor() {
  // Чтение аналогового датчика влажности почвы
  // Читаем 5 раз и усредняем для стабильности
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += analogRead(SOIL_PIN);
    delay(10);
  }
  
  int average = sum / 5;
  readings.soilSensorOK = true;
  
  return average;
}

void SensorManager::readCurrentTime() {
  // Заглушка для DS1307
  // В реальности:
  // time_t now = rtc.now();
  // readings.hour = hour(now);
  // readings.minute = minute(now);
  // readings.second = second(now);
  
  // Симуляция: увеличиваем время на 1 секунду каждый вызов
  // (В реальности будем использовать миллис() для имитации)
  static unsigned long lastUpdate = 0;
  static uint8_t simHour = 12;
  static uint8_t simMinute = 0;
  static uint8_t simSecond = 0;
  
  if (millis() - lastUpdate >= 1000) {
    simSecond++;
    if (simSecond >= 60) {
      simSecond = 0;
      simMinute++;
      if (simMinute >= 60) {
        simMinute = 0;
        simHour++;
        if (simHour >= 24) {
          simHour = 0;
        }
      }
    }
    lastUpdate = millis();
  }
  
  readings.hour = simHour;
  readings.minute = simMinute;
  readings.second = simSecond;
}

// === Индивидуальные методы чтения ===

float SensorManager::readLightLevel() {
  return readLightSensor();
}

float SensorManager::readWaterLevel() {
  return calculateVolume(readDistance());
}

void SensorManager::readAirData(float &temp, float &hum) {
  temp = readings.temperature;
  hum = readings.humidity;
}

void SensorManager::readAirQuality(int &aqi, uint16_t &tvoc, uint16_t &eco2) {
  aqi = readings.airQualityIndex;
  tvoc = readings.tvoc;
  eco2 = readings.eco2;
}

int SensorManager::readSoilMoisture() {
  return mapFloat(
    readSoilSensor(),
    soilWetValue,
    soilDryValue,
    100.0,
    0.0
  );
}

void SensorManager::readTime(uint8_t &h, uint8_t &m, uint8_t &s) {
  h = readings.hour;
  m = readings.minute;
  s = readings.second;
}

// === Калибровка ===

void SensorManager::calibrateSoilSensor(int dryValue, int wetValue) {
  soilDryValue = dryValue;
  soilWetValue = wetValue;
  
  Serial.print(F("Калибровка датчика почвы: "));
  Serial.print(F("Сухо="));
  Serial.print(dryValue);
  Serial.print(F(", Мокро="));
  Serial.println(wetValue);
}

// === Вспомогательные методы ===

float SensorManager::mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  // Аналог map() но для float
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}