#include "main.h"

// Глобальные объекты
SensorManager sensors;
DeviceManager devices(LIGHT_PIN, FAN_PIN, PUMP_PIN);
GreenhouseDisplay display(0x27, 16, 2);

bool systemAutoMode = true;

void runAutoMode();

void setup() {
    Serial.begin(9600);
    sensors.init();
    devices.init();
    display.begin();
}

void loop() {
    static uint32_t lastSensorUpdate = 0;
    if (millis() - lastSensorUpdate > 1000) {
        sensors.update_all();
        updateDisplayWithSensorData();
        lastSensorUpdate = millis();
    }
    // 3. Обновление UI
    display.update();
    //4. Обновление устройств (для насоса с автостопом)
    devices.update();

    if (systemAutoMode) {
        runAutoMode();
    }
    delay(50);
}

// TODO: complete autologic with watering, complete error handler
void runAutoMode() {

    static uint32_t lastAutoAction = 0;
    if (millis() - lastAutoAction > 10000) {  // Каждые 10 секунд
        lastAutoAction = millis();

        if ((sensors.get_light_level() < 50 || sensors.get_hour() > 20 || sensors.get_hour() < 8)&& !devices.isLightOn()) {
            devices.setLight(true);
        } else if ((sensors.get_light_level() > 500 && sensors.get_hour() > 8) && devices.isLightOn()) {
            devices.setLight(false);
        }

        if (((sensors.get_air_temp() > 28) || (sensors.get_air_humidity() > 75) || (sensors.get_air_quality() > 60)) && !devices.isFanOn()) {
            devices.setFan(true);
        } else if ((sensors.get_air_temp() < 25  || (sensors.get_air_humidity() < 60) || (sensors.get_air_quality() < 50)) && devices.isFanOn()) {
            devices.setFan(false);
        }

        //TODO:Fix magic number usage
        if (((sensors.get_soil_moisture_1() < 20))) {
            //devices.startPump(100);
            display.showMessage("WATER", "NOW",50000);
        }

    }
}

void updateDisplayWithSensorData() {
    // Передача всех данных сенсоров
    display.setTemperature(sensors.get_air_temp());
    display.setHumidity(sensors.get_air_humidity());
    display.setSoilMoisture1(sensors.get_soil_moisture_1());
    display.setSoilMoisture2(sensors.get_soil_moisture_2());
    display.setLightLevel(sensors.get_light_level());
    display.setWaterVolume(sensors.get_water_volume());
    display.setAirQuality(sensors.get_air_quality());
    display.setTime(sensors.get_hour(),sensors.get_minute());
    // Передача состояния устройств
    display.setLightState(devices.isLightOn());
    display.setFanState(devices.isFanOn());
    display.setPumpState(devices.isPumpOn());
    display.setDate(sensors.get_day(), sensors.get_month(), sensors.get_year());
    // Установка режима работы
    display.setAutoMode(systemAutoMode);
}