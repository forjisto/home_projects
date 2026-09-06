#pragma once
#include <Arduino.h>
#include <stdint.h>
#include "pins.h"

namespace Set_sound
{
    constexpr int DURATION = 1000; // длительность
    constexpr int PAUSE = 100;
    constexpr int FREQ_HZ = 1900; // частота баззера
    constexpr int FREQ_INTERVAL = 300;
    constexpr float TIME_SOUND = (DURATION - 2 * PAUSE) / 3;
    constexpr int TONE_1 = FREQ_HZ + FREQ_INTERVAL;     // тон выше
    constexpr int TONE_2 = FREQ_HZ + 2 * FREQ_INTERVAL; // тон еще выше
}

// звук приветсвия(запуска)
void start_sound_welcome()
{
    tone(Indication::BUZZER, Set_sound::FREQ_HZ, Set_sound::TIME_SOUND);
    delay(Set_sound::PAUSE);

    tone(Indication::BUZZER, Set_sound::TONE_1, Set_sound::TIME_SOUND);
    delay(Set_sound::PAUSE);

    tone(Indication::BUZZER, Set_sound::TONE_2, Set_sound::TIME_SOUND);
    delay(Set_sound::PAUSE);

    noTone(Indication::BUZZER);
    digitalWrite(Indication::LED_BASE_BOARD, LOW); // инвертированный светодиод
}

//  мелодии включения
void set_melody_on()
{
    // tone(pin, тон, длительность тона)
    const int pause_tone = 40;
    const int pause_led = 40;

    tone(Indication::BUZZER, Set_sound::FREQ_HZ, pause_tone);    // пик 1
    led_base_board(Indication::LED_BASE_BOARD, HIGH, pause_led); // вспышка 1 на плате вкл
    delay(10.0);

    led_base_board(Indication::LED_BASE_BOARD, LOW, 200);
    tone(Indication::BUZZER, Set_sound::FREQ_HZ, pause_tone); // пик 2

    led_base_board(Indication::LED_BASE_BOARD, HIGH, pause_led);
    led_base_board(Indication::LED_BASE_BOARD, LOW, 0);
}

// мелодии выключения
void set_melody_off()
{
    tone(Indication::BUZZER, Set_sound::FREQ_HZ, 200.0); // тон 2000 лучше слышно
    led_base_board(Indication::LED_BASE_BOARD, HIGH, 400);
    led_base_board(Indication::LED_BASE_BOARD, LOW, 0);
}

void enable_sound(bool val)
{
    if (val)
        set_melody_on();
    else
        set_melody_off();
}