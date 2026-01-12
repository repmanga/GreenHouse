#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "SensorManager.h"
#include "DeviceManager.h"

// Структура для хранения настроек
struct UserSettings {
    struct Thresholds {
        float minTemp = 18.0;
        float maxTemp = 28.0;
        float minHumidity = 40.0;
        float maxHumidity = 80.0;
        uint16_t minLight = 1000;
        uint16_t maxLight = 20000;
        uint8_t minSoilMoisture = 40;
    } thresholds;

    struct WateringSchedule {
        uint8_t hour;
        uint8_t minute;
        uint16_t volumeML;
        bool enabled;
    };

    WateringSchedule schedules[3] = {
            {8, 0, 200, true},
            {14, 0, 150, true},
            {20, 0, 200, false}
    };

    struct LightControl {
        bool useTime = true;
        uint8_t onHour = 6;
        uint8_t onMinute = 0;
        uint8_t offHour = 22;
        uint8_t offMinute = 0;
        uint16_t lightThreshold = 5000;
    } lightControl;

    String plantPreset = "Default";
    bool saved = false;

    // Методы для работы с EEPROM
    void saveToEEPROM(int address);
    void loadFromEEPROM(int address);
};

// Класс для обработки энкодера
class Encoder {
private:
    uint8_t clkPin, dtPin, btnPin;
    int lastState;
    unsigned long lastPressTime;
    bool buttonPressed;

public:
    Encoder(uint8_t clk, uint8_t dt, uint8_t btn);
    void init();
    int getRotation(); // -1, 0, 1
    bool isClicked();
    bool isPressed();
    bool isLongPress();
};

// Главный класс UI контроллера
class UIController {
private:
    // Оборудование
    LiquidCrystal_I2C* lcd;
    Encoder* encoder;
    SensorManager* sensors;
    DeviceManager* devices;

    // Конфигурация
    uint8_t ledPin;
    uint8_t buzzerPin;

    // Состояние системы
    struct SystemState {
        bool autoMode;
        bool ledOn;
        bool fanOn;
        bool pumpOn;
        uint8_t currentError;
        String errorMessage;
    } state;

    // Настройки пользователя
    UserSettings settings;

    // Меню и навигация
    enum MenuPage {
        PAGE_MAIN,
        PAGE_LIGHT_CONTROL,
        PAGE_FAN_CONTROL,
        PAGE_WATER_CONTROL,
        PAGE_SET_DATETIME,
        PAGE_SET_THRESHOLDS,
        PAGE_SENSOR_INFO,
        PAGE_SCHEDULE,
        PAGE_LIGHT_MODE,
        PAGE_PRESETS,
        PAGE_AUTO_MODE,
        PAGE_ERRORS
    };

    enum MenuItem {
        // Главное меню
        ITEM_LIGHT = 0,
        ITEM_FAN,
        ITEM_WATER,
        ITEM_SENSORS,
        ITEM_SETTINGS,
        ITEM_AUTO_MODE,

        // Меню настроек
        SETTING_DATETIME,
        SETTING_THRESHOLDS,
        SETTING_SCHEDULE,
        SETTING_LIGHT_MODE,
        SETTING_PRESETS,
        SETTING_SAVE,
        SETTING_BACK
    };

    MenuPage currentPage;
    MenuPage previousPage;
    uint8_t menuPosition;
    uint8_t cursorPosition;
    bool editingMode;
    int editValue;
    uint8_t editStep;
    String editBuffer;

    // Таймеры
    unsigned long lastDisplayUpdate;
    unsigned long lastErrorCheck;
    unsigned long lastBlink;
    bool blinkState;

    // Ошибки
    enum ErrorCode {
        ERROR_NONE = 0,
        ERROR_WATER_EMPTY,
        ERROR_PUMP_FAILURE,
        ERROR_LOW_TEMP,
        ERROR_HIGH_HUMIDITY,
        ERROR_SENSOR_FAIL,
        ERROR_COUNT
    };

    struct Error {
        ErrorCode code;
        String message;
        bool active;
        unsigned long firstDetected;
    };

    Error errors[ERROR_COUNT];

    // Приватные методы
    void updateSensors();
    void checkErrors();
    void updateDisplay();
    void drawMainMenu();
    void drawSettingsMenu();
    void drawSensorInfo();
    void drawEditScreen();
    void drawErrorScreen();
    void handleEncoder();
    void handleButton();
    void executeCommand();
    void navigateTo(MenuPage page);
    void editParameter(const String& title, int minVal, int maxVal, int step);
    void saveSettings();
    void loadSettings();
    void beep(uint8_t count, uint16_t duration = 100);
    void showMessage(const String& line1, const String& line2 = "", uint16_t duration = 2000);

    // Методы редактирования
    void editDateTime();
    void editThresholds();
    void editWaterVolume();
    void editSchedule();
    void editLightMode();

public:
    UIController(
            uint8_t lcdAddr, uint8_t lcdCols, uint8_t lcdRows,
            uint8_t encClk, uint8_t encDt, uint8_t encBtn,
            uint8_t led, uint8_t buzzer,
            SensorManager* sensorMgr, DeviceManager* deviceMgr
    );

    void init();
    void update();

    // Внешний контроль
    void setAutoMode(bool enable);
    void setLed(bool on);
    void setFan(bool on);
    void waterPlants(uint16_t volumeML);
    void setDateTime(uint8_t hour, uint8_t minute, uint8_t day, uint8_t month, uint16_t year);

    // Геттеры
    bool isAutoMode() const { return state.autoMode; }
    UserSettings& getSettings() { return settings; }
    bool hasError() const { return state.currentError != ERROR_NONE; }
    String getErrorMessage() const { return state.errorMessage; }

    // Статические тексты
    static const char* MAIN_MENU_ITEMS[];
    static const char* SETTINGS_MENU_ITEMS[];
    static const char* ERROR_MESSAGES[];
    static const char* PLANT_PRESETS[];
};

#endif