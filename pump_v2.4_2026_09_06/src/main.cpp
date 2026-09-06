#include <Arduino.h>

#include <config/pins.h>
#include <config/sounds.h>
#include <config/timer.h>

// функция настройки включения светодиодов показывающих статус насоса
// красный - ВКЛ
// зеленый - ВЫКЛ
void indication_led(uint8_t val)
{
  digitalWrite(Indication::LED_BASE_BOARD, val);
  digitalWrite(Indication::LED_RED_ON, val);
  digitalWrite(Indication::LED_GREEN_OFF, !val);
}

// релейный модуль
void relay_module(bool val)
{
  digitalWrite(RelayModule::RELAY_1, val);
  delay(RelayModule::PAUSE_MS);
  digitalWrite(RelayModule::RELAY_2, val);
}

// управление  насосом и индикацией
void pump_and_indication(bool val)
{
  indication_led(val);
  enable_sound(val);
  relay_module(val);
}

// тип нажатий кнопки
enum PressType
{
  NONE,
  SINGLE, // одно нажатие
  DOUBLE  // двойное нажатие
};

// режим насоса
enum PumpMode
{
  OFF,
  ON
};

// функция читающая состояние кнопки и возвращяющая значения для передачи в функцию исполнителя
PressType getPressType()
{

  StateSwitch.reading = digitalRead(Switch::SWITH);

  if (StateSwitch.reading && !StateSwitch.last_reading)
  {
    Serial.println("нажал");
    StateSwitch.on_time = millis();
  }

  if (!StateSwitch.reading && StateSwitch.last_reading)
  {
    Serial.println("отпустил");
    // если было одно нажатие
    if (((millis() - StateSwitch.on_time) > Switch::BOUNCE_TIME_MS))
    { // если было 2 нажатия
      if ((millis() - StateSwitch.last_swith_time) >= Switch::DOUBLE_TIME_MS)
      {
        StateSwitch.last_swith_time = millis();
        StateSwitch.single = true;
        // StateSwitch.count = 1;

        Serial.println("DOUBLE");
      }
      else
      {
        // StateSwitch.count++;

        StateSwitch.last_swith_time = millis();

        StateSwitch.single = false;
        StateSwitch.multi = true;

        Serial.println("single");
      }
    }
  }
  StateSwitch.last_reading = StateSwitch.reading;

  //===============================
  // ДЕЙСТВИЯ ПО КОЛИЧЕСТВУ НАЖАТИЙ
  //===============================

  bool state_on = (!StateSwitch.turbo_off &&
                   StateSwitch.multi &&
                   (millis() - StateSwitch.last_swith_time) > Switch::DOUBLE_TIME_MS);

  bool state_off = (StateSwitch.turbo_off &&
                    StateSwitch.multi &&
                    StateSwitch.reading &&
                    (millis() - StateSwitch.on_time) > Switch::BOUNCE_TIME_MS);

  if (state_on)
  {

    StateSwitch.single = false;
    StateSwitch.turbo_off = true;
    return DOUBLE;
  }

  // 5. когда нажатие было одно, то выкл

  if (state_off)
  {

    StateSwitch.single = false;
    StateSwitch.multi = false;
    StateSwitch.turbo_off = false;
    return SINGLE;
  }
  return NONE;
}

// управление насосом
void setPumpMode(PumpMode mode)
{
  switch (mode)
  {
  case OFF:
    pump_and_indication(false);
    Timer_nasos::stateTimer = false;
    Serial.println("таймер выкл");
    break;

  case ON:
    pump_and_indication(true);
    Timer_nasos::pumpStartTime = millis();
    Timer_nasos::stateTimer = true;
    Serial.println("таймер запушен");
    break;
  }
}

// проверка таймера
void chekTimer()
{
  if (Timer_nasos::stateTimer && (millis() - Timer_nasos::pumpStartTime >= Timer_nasos::DURATION_MS))
  {
    Serial.println("время вышло");
    setPumpMode(OFF);
  }
}

// функция исполнитель. включаеет насос
void handleButton(PressType press)
{
  switch (press)
  {
  case SINGLE:
    setPumpMode(OFF);
    break;
  case DOUBLE:
    setPumpMode(ON);
    break;
  case NONE:
    break;
  }
}

void setup()
{
  Serial.begin(9600);
  delay(500);

  pinMode(Indication::LED_BASE_BOARD, OUTPUT);
  pinMode(Indication::LED_RED_ON, OUTPUT);
  pinMode(Indication::LED_GREEN_OFF, OUTPUT);
  // pinMode(Indication::LED_BLUE_WIFI, OUTPUT);
  pinMode(RelayModule::RELAY_1, OUTPUT);
  pinMode(RelayModule::RELAY_2, OUTPUT);
  pinMode(Switch::SWITH, OUTPUT);

  indication_led(LOW);
  start_sound_welcome();
}

void loop()
{
  chekTimer();

  PressType press = getPressType();
  if (press != NONE)
    handleButton(press);
}
