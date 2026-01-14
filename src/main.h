#ifndef INCLUDE_MAIN
#define INCLUDE_MAIN

#include <Arduino.h>
#include "SensorManager.h"
#include "DeviceManager.h"
#include "SimpleLCD.h"

const uint8_t LIGHT_PIN = 6;
const uint8_t FAN_PIN = 5;
const uint8_t PUMP_PIN = 7;
// Пины
/*
const uint8_t ENC_CLK = 2;
const uint8_t ENC_DT = 3;
const uint8_t ENC_SW = 4;
*/
void updateDisplayWithSensorData();

#endif