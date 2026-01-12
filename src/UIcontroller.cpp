#include "UIController.h"

// Пины для UI
const uint8_t ENC_CLK = 2;
const uint8_t ENC_DT = 3;
const uint8_t ENC_SW = 4;
const uint8_t LCD_ADDR = 0x27;
const uint8_t LCD_COLS = 16;
const uint8_t LCD_ROWS = 4;
const uint8_t BUZZER_PIN = 8;
const uint8_t LED_PIN = 9;

// Пины устройств
const uint8_t LIGHT_PIN = 5;
const uint8_t FAN_PIN = 6;
const uint8_t PUMP_PIN = 7;

// Глобальные объекты
SensorManager sensors;
DeviceManager devices(LIGHT_PIN, FAN_PIN, PUMP_PIN);
UIController ui(LCD_ADDR, LCD_COLS, LCD_ROWS,
                ENC_CLK, ENC_DT, ENC_SW,
                LED_PIN, BUZZER_PIN,
                &sensors, &devices);

// Данные для автоматического режима
struct AutoModeData {
    uint32_t lastWaterTime = 0;
    uint32_t lastLightCheck = 0;
    uint32_t lastFanCheck = 0;
    uint32_t lastScheduleCheck = 0;
} autoData;

void setup() {
    Serial.begin(9600);

    // Инициализация
    sensors.init();
    devices.init();
    ui.init();

    ui.showMessage("Smart", "Greenhouse");
    delay(2000);
}

void loop() {
    static uint32_t lastSensorUpdate = 0;

    // Обновление датчиков каждые 5 секунд
    if (millis() - lastSensorUpdate > 5000) {
        sensors.update_all();
        lastSensorUpdate = millis();
    }

    // Обновление UI
    ui.update();

    // Обновление устройств (насос с автостопом)
    devices.update();

    // Выбор режима работы
    if (ui.isAutoMode()) {
        runAutoMode();
    }

    delay(50);
}

void runAutoMode() {
    uint32_t currentTime = millis();

    // Проверка полива по расписанию (каждые 10 минут)
    if (currentTime - autoData.lastScheduleCheck > 600000) {
        checkWateringSchedule();
        autoData.lastScheduleCheck = currentTime;
    }

    // Проверка освещения (каждые 30 секунд)
    if (currentTime - autoData.lastLightCheck > 30000) {
        controlLightAuto();
        autoData.lastLightCheck = currentTime;
    }

    // Проверка вентиляции (каждые 30 секунд)
    if (currentTime - autoData.lastFanCheck > 30000) {
        controlFanAuto();
        autoData.lastFanCheck = currentTime;
    }

    // Полив по влажности почвы (каждую минуту)
    if (currentTime - autoData.lastWaterTime > 60000) {
        checkSoilMoisture();
        autoData.lastWaterTime = currentTime;
    }
}

void checkWateringSchedule() {
    // Получаем текущее время
    tmElements_t tm;
    if (DS1307RTC::read(tm)) {
        uint8_t currentHour = tm.Hour;
        uint8_t currentMinute = tm.Minute;

        // Проверка расписаний
        UserSettings& settings = ui.getSettings();
        for (int i = 0; i < 3; i++) {
            if (settings.schedules[i].enabled &&
                settings.schedules[i].hour == currentHour &&
                settings.schedules[i].minute == currentMinute) {

                devices.startPump(settings.schedules[i].volumeML);
                break;
            }
        }
    }
}

void controlLightAuto() {
    UserSettings& settings = ui.getSettings();

    if (settings.lightControl.useTime) {
        // По времени
        tmElements_t tm;
        if (DS1307RTC::read(tm)) {
            uint16_t currentMinute = tm.Hour * 60 + tm.Minute;
            uint16_t onMinute = settings.lightControl.onHour * 60 + settings.lightControl.onMinute;
            uint16_t offMinute = settings.lightControl.offHour * 60 + settings.lightControl.offMinute;

            if (onMinute < offMinute) {
                // В течение одного дня
                devices.setLight(currentMinute >= onMinute && currentMinute < offMinute);
            } else {
                // Переход через полночь
                devices.setLight(currentMinute >= onMinute || currentMinute < offMinute);
            }
        }
    } else {
        // По освещенности
        float lightLevel = sensors.get_light_level();
        if (lightLevel < settings.lightControl.lightThreshold && !devices.isLightOn()) {
            devices.setLight(true);
        } else if (lightLevel >= settings.lightControl.lightThreshold && devices.isLightOn()) {
            devices.setLight(false);
        }
    }
}

void controlFanAuto() {
    UserSettings& settings = ui.getSettings();

    float temperature = sensors.get_air_temp();
    float humidity = sensors.get_air_humidity();
    float airQuality = sensors.get_air_quality();

    // Включение вентилятора при превышении порогов
    if ((temperature > settings.thresholds.maxTemp ||
         humidity > settings.thresholds.maxHumidity ||
         airQuality > 60) && !devices.isFanOn()) {
        devices.setFan(true);
    }

    // Выключение вентилятора при нормализации
    if (temperature < settings.thresholds.maxTemp - 2 &&
        humidity < settings.thresholds.maxHumidity - 10 &&
        airQuality < 50 && devices.isFanOn()) {
        devices.setFan(false);
    }
}

void checkSoilMoisture() {
    UserSettings& settings = ui.getSettings();

    // Средняя влажность почвы
    uint16_t avgMoisture = (sensors.get_soil_moisture_1() + sensors.get_soil_moisture_2()) / 2;

    if (avgMoisture < settings.thresholds.minSoilMoisture && !devices.isPumpOn()) {
        // Полив 100 мл при низкой влажности
        devices.startPump(100);
    }
}