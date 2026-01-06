#include "DeviceManager.h"

DeviceManager::DeviceManager(uint8_t lightPin, uint8_t fanPin, uint8_t pumpPin, uint8_t ledPin) : lightPin(lightPin), fanPin(fanPin), pumpPin(pumpPin), ledPin(ledPin)
{
    lightState = false;
    fanState = false;
    pumpState = false;
    ledState = false;

    pumpFlowRate = 100; // 100 мл/мин
    pumpAutoStop = false;
    pumpStartTime = 0;
    pumpDuration = 0;

    lastBlinkTime = 0;
    blinkCount = 0;
    blinkState = false;
}

void DeviceManager::init()
{
    pinMode(lightPin, OUTPUT);
    pinMode(fanPin, OUTPUT);
    pinMode(pumpPin, OUTPUT);
    pinMode(ledPin, OUTPUT);

    digitalWrite(lightPin, LOW);
    digitalWrite(fanPin, LOW);
    digitalWrite(pumpPin, LOW);
    digitalWrite(ledPin, LOW);
}

void DeviceManager::setLight(bool state)
{
    lightState = state;
    digitalWrite(lightPin, state ? HIGH : LOW);
}

void DeviceManager::setFan(bool state)
{
    fanState = state;
    digitalWrite(fanPin, state ? HIGH : LOW);
}

void DeviceManager::startPump(uint16_t ml)
{
    if (ml == 0)
    {
        return;
    }
    pumpDuration = calculatePumpTime(ml);
    pumpStartTime = millis();
    pumpAutoStop = true;
    pumpState = true;
    digitalWrite(pumpPin, HIGH);
}

void DeviceManager::stopPump()
{
    pumpState = false;
    digitalWrite(pumpPin, LOW);
    pumpAutoStop = false;
    uint32_t actualTime = millis() - pumpStartTime;
}

void DeviceManager::blinkLED(uint8_t count, uint16_t duration)
{
    blinkCount = count * 2;
    lastBlinkTime = millis();
    ledState = true;
    digitalWrite(ledPin, HIGH);
}

void DeviceManager::setLED(bool state)
{
    ledState = state;
    blinkCount = 0;
    digitalWrite(ledPin, state ? HIGH : LOW);
}

void DeviceManager::update()
{
    updatePump();
    updateLED();
}

void DeviceManager::updatePump()
{
    if (!pumpState || !pumpAutoStop)
        return;

    uint32_t currentTime = millis();

    if (currentTime < pumpStartTime)
    {
        pumpStartTime = currentTime;
        return;
    }

    if (currentTime - pumpStartTime >= pumpDuration)
    {
        stopPump();
    }
}

void DeviceManager::updateLED()
{
    if (blinkCount == 0)
        return;

    uint32_t currentTime = millis();

    if (currentTime < lastBlinkTime)
    {
        lastBlinkTime = currentTime;
        return;
    }

    if (currentTime - lastBlinkTime >= 200)
    {
        blinkState = !blinkState;
        digitalWrite(ledPin, blinkState ? HIGH : LOW);
        lastBlinkTime = currentTime;

        if (--blinkCount == 0)
        {
            ledState = false;
        }
    }
}

uint32_t DeviceManager::calculatePumpTime(uint16_t ml)
{
    return (static_cast<uint32_t>(ml) * 60000UL) / pumpFlowRate;
}