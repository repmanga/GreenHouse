#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
class DeviceManager
{
private:
    // Пины устройств
    uint8_t lightPin;
    uint8_t fanPin;
    uint8_t pumpPin;
    uint8_t ledPin;

    // Состояния устройств
    bool lightState;
    bool fanState;
    bool pumpState;
    bool ledState;

    // Управление насосом
    uint32_t pumpStartTime;
    uint32_t pumpDuration;
    bool pumpAutoStop;
    uint16_t pumpFlowRate; // мл в минуту

    // Сигнализация
    uint32_t lastBlinkTime;
    uint8_t blinkCount;
    bool blinkState;

    // Приватные методы
    void updatePump();
    void updateLED();
    uint32_t calculatePumpTime(uint16_t ml);

public:
    // Конструктор с настройкой пинов
    DeviceManager(uint8_t lightPin, uint8_t fanPin, uint8_t pumpPin, uint8_t ledPin);

    // Инициализация
    void init();

    // Управление устройствами
    void setLight(bool state);
    void setFan(bool state);
    void startPump(uint16_t ml); // Полив заданного объема в мл
    void stopPump();

    // Сигнализация
    void blinkLED(uint8_t count = 3, uint16_t duration = 200);
    void setLED(bool state);

    // Статус
    bool isLightOn() const { return lightState; }
    bool isFanOn() const { return fanState; }
    bool isPumpOn() const { return pumpState; }
    bool isLEDOn() const { return ledState; }

    // Обновление состояния (вызывать в loop для управления насосом)
    void update();

    // Настройка производительности насоса (мл/мин)
    void setPumpFlowRate(uint16_t mlPerMin) { pumpFlowRate = mlPerMin; }
};

#endif