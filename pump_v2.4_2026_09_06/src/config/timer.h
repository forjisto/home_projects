#pragma once
#include <Arduino.h>
#include <stdint.h>

// таймер для насоса
namespace Timer_nasos
{
    // constexpr int CHEK_PIN = 21;
    constexpr uint32_t MIN_TO_MS = 60 * 1000U;                         // число для конвертации минут в мс
    constexpr uint32_t SET_DURATION_MINUTES = 15;                      // длительность таймера в минутах
    constexpr uint32_t DURATION_MS = SET_DURATION_MINUTES * MIN_TO_MS; // итоговая длительность в мс. Для изменеия времени измените SET_DURATION_MINUTES
    unsigned long pumpStartTime = 0;                                   // хранилище времени для отсчета до BOUND_MINUTES
    bool stateTimer;

}