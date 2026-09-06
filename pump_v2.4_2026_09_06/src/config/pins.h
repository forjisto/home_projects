
#pragma once
#include <stdint.h>
#include <Arduino.h>

// любая индикация
namespace Indication
{
    constexpr int LED_BASE_BOARD = D13; // светодиод на плате
    constexpr int LED_GREEN_OFF = D9;  // состояние выключено
    constexpr int LED_RED_ON = D10;     // состояние включено
    constexpr int BUZZER = D11;         // звуковая индикация

}

// о реле
namespace RelayModule
{
    constexpr int RELAY_1 = D8;
    constexpr int RELAY_2 = D7;
    constexpr int PAUSE_MS = 200; // задержка между включением 1го и 2го реле в мс
    bool state_relays = false;    // состояние реле

}

// кнопка управления
namespace Switch
{
    constexpr int SWITH = D12;
    constexpr int DEFAULT_BOUNCE_TIME_MS = 60;   // значение по умолчанию примерно 60 -150 ms
    constexpr int DEFAULT_DOUBLE_TIME_MS = 1300; // значение по умолчанию
    constexpr int BOUNCE_TIME_MS = 100;          // от дребезга
    constexpr int DOUBLE_TIME_MS = 1300;         // 1300 . время после после которого нажатие можно считать двойным

}

// состояние кнопки
struct ConfigStateSwitch
{
    uint16_t on_time = 0;         // время вкоючения
    uint16_t last_swith_time = 0; // время последенего нажатия
    // uint8_t count = 0;            // счетчик
    bool reading = false;      // текущее сотояние
    bool last_reading = false; // флаг последнего состояния
    bool single = false;       // флаг одиночного нажатия
    bool multi = false;        // флаг двойного нажатия
    bool turbo_off = false;    // флаг корректровки для быстрого отключения
    bool mqtt_flag = false;    // флаг mqtt
};

// состояние кнопки
ConfigStateSwitch StateSwitch;

void led_base_board(int pin, int var, uint16_t duration)
{
    digitalWrite(pin, var);
    delay(duration);
}
