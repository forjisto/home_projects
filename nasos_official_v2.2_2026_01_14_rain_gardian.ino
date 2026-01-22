/*
Изменения:

1. изменеие паузы между звуковыми сигналами в звуке запуска насоса
2. 19-03-2025. Левое реле не замыкае контакты с первого раза. Изменяю паузу
между включениями реле. Гипотеза: контакты левого реле ловят искру во время
замыкания и размыкания - оно является вторым реле , правое - первым. Решение -
меняю порядок запуска реле.
3. 26-03-2025. Замена реле на бистабильное реле в связке с мосфетом IRF520 =>
переписан код включения реле1; реле2 закомментировал.
4. 29-03-2025. Исправил ошибку в коде. изменил задержку для подавления дребезка
до 100 мс.
5. 14-01-2026. Свел общие переменные и константы в структуры.
Вернул управление двумя реле без мосфета; бистабильные реле будут заменены на обычные с током 30 А. 
Добавил индикацию о мокрой кнопке (rain_gardian) - при намокании кнопки контакты ее замкнутся - тогда начнется мигание светодиодами .
Rain_gardian служит для обнаружении НЕ герметичности/несправности кнопки (кнопка в пульте используется водозащищенная) или сильной влажности в корпусе.

*/

const int BUTTON = D8;

struct set_indication {
  const int LED_BOARD = 13;
  const int GREEN = D5;
  const int RED = D6;
  const int BUZZER = D7;
};
set_indication indication;

struct set_control {
  const int RELAY_1 = D11;
  const int RELAY_2 = D12;
  int pause = 200;
};
set_control control_relay;

//---------------------------
// sound start
const int total_second = 1000;
const int pause_sound = 100;
int time_sound = round((total_second - 2 * pause_sound) / 3);
const int interval = 300;

const int ton = 1900;  // 1900
int tone_1 = ton + interval;
int tone_2 = round(ton + 2 * interval);

//---------------------------

// кнопки

struct pause {
  const int BOUNCE_TIME = 100;   // 60 по умолчанию.  Задержка для подавления дребезга
  const int DOUBLE_TIME = 1300;  // 1300 по умолчанию. Время, в течение которого нажатия можно считать двойным
};

pause button_pause;

// дождева защита
struct set_rain_gardian {
  int duration_push = 5000;
  long start_time_push;
  bool status = false;
};
set_rain_gardian rain_gardian;
//

int i = 0;  //счетчик

struct set_flag {
  bool reading;
  bool last_reading = false;  // флаг предыдущего состояния кнопки
  bool single = false;        // флаг состояния "краткое нажатие"
  bool multi = false;         // флаг состояния "двойное нажатие"
  bool correct = false;       // флаг корректировки (возможно не нужен)
};
set_flag flag_button;



long on_time = 0;           // переменная обработки временного интервала
long last_switch_time = 0;  // переменная времени предыдущего переключения состояния

void setup() {
  Serial.begin(9600);

  delay(500);  // пауза
  pinMode(BUTTON, INPUT);
  pinMode(indication.LED_BOARD, OUTPUT);
  pinMode(indication.GREEN, OUTPUT);
  pinMode(indication.RED, OUTPUT);
  pinMode(control_relay.RELAY_1, OUTPUT);
  pinMode(control_relay.RELAY_2, OUTPUT);

  // запуск приветсвия включения
  start_light_module();
  start_sound_welcome();
}

//индикация показывающая что плата включена
void start_light_module() {
  digitalWrite(indication.LED_BOARD, HIGH);
  digitalWrite(indication.GREEN, HIGH);
}

//звук включения платы
void start_sound_welcome() {
  tone(indication.BUZZER, ton, time_sound);
  delay(pause_sound);

  tone(indication.BUZZER, tone_1, time_sound);
  delay(pause_sound);

  tone(indication.BUZZER, tone_2, time_sound);
  delay(pause_sound);

  noTone(indication.BUZZER);
  digitalWrite(indication.LED_BOARD, LOW);
}
/*
millis() - это функция,
 которая возвращает количество миллисекунд
  с момента начала выполнения текущей программы - это как настенные часы
*/

void loop() {

  flag_button.reading = digitalRead(BUTTON);  // читается состояние нажатия кнопки

  // 1. нажал кнопку
  // проверка первичного нажатия
  if (flag_button.reading && !flag_button.last_reading) {  // если состояние кнопки true и последенее
                                             // состояние кноки false, то
    on_time = millis();                      // записыавем время нажатия
    rain_gardian.start_time_push = on_time;
  }

  // --- дождевая защита ---
  if (millis() - rain_gardian.start_time_push >= rain_gardian.duration_push && flag_button.reading ) {
    Serial.println("Тревога");

    blink(500);  //мигалка
    rain_gardian_sound(100);
    rain_gardian.status = true;
  }

  if (rain_gardian.status) {
    digitalWrite(indication.LED_BOARD, LOW);
    rain_gardian.status = false;
  }

  // ----------------------

  // 2. отпустил кнопку
  if (!flag_button.reading && flag_button.last_reading) {



    // 3. если с момента как отпустил кнопку прошло достаточно времени, то
    // считается, что нажал кнопку 1 раз, тогда
    if (((millis() - on_time) > button_pause.BOUNCE_TIME)) {

      // СЧЕТЧИК НАЖАТИЙ
      // 3.1. и с момента как нажал 1 раз прошло больше времени заькоторе можно
      // счиать двойное нажатие, то 3.2. если было 2е нажатие то условие это не
      // выполнится, тогда п.3.3
      if ((millis() - last_switch_time) >= button_pause.DOUBLE_TIME) {  // если (время работы - последнеевремянажтия(=0
                                                                        // вначале))>=времени 2го нажатия, то

        last_switch_time = millis();  // начинает считать время последнего нажатия --> см. п.3.2
        flag_button.single = true;           // тогда флаг одного нажатия = 1
        i = 1;                        // счетчик нажатий = 1

        // 3.3. тогда
      } else {

        i++;  // счетчик нажатий прибавляется 1

        last_switch_time = millis();  // начинает считать время последнего
                                      // нажатия (для большего числа нажатий)
        flag_button.single = false;          // тогда флаг 1 нажатия 0
        flag_button.multi = true;
        // flag_button.correct = true;  // тогда флаг 2го нажатия 1
      }
    }
  }
  // 4. замена состояния кнопки
  flag_button.last_reading = flag_button.reading;

  // ДЕЙСТВИЯ ПО КОЛИЧЕСТВУ НАЖАТИЙ

  // 6. было 2 - включаем  насос
  if (!flag_button.correct && flag_button.multi && (millis() - last_switch_time) > button_pause.DOUBLE_TIME) {
    nasos(HIGH);
    flag_button.single = false;

    flag_button.correct = true;
  }

  // 5. когда нажатие было одно, то выкл
  if (flag_button.correct && flag_button.multi && flag_button.reading && (millis() - on_time) > button_pause.BOUNCE_TIME) {
    nasos(LOW);
    flag_button.single = false;
    flag_button.multi = false;

    flag_button.correct = false;
    
  }
}
// конец петли

//дождевая защита: мигание при намокании кнопки полностью
void blink(int pause) {
  digitalWrite(indication.GREEN, LOW);
  digitalWrite(indication.LED_BOARD, LOW);
  delay(pause);

  digitalWrite(indication.GREEN, HIGH);
  digitalWrite(indication.LED_BOARD, HIGH);
  delay(pause);
}



// функция настройки включения светодиодов показывающих статус насоса
// красный - ВКЛ
// зеленый - ВЫКЛ
void led_status_power(int logical_level) {
  digitalWrite(indication.RED, logical_level);
  digitalWrite(indication.GREEN, !logical_level);
}

// функция включения насоса
// тест функция
//  nasos(HIGH)
void nasos(int power_status) {
    led_status_power(power_status);
    melody(power_status);
    relays(power_status);
}

// настройка реле
void relays(int power_status) {
  digitalWrite(control_relay.RELAY_1, power_status);
  delay(control_relay.pause);
  digitalWrite(control_relay.RELAY_2, power_status);
}

void led_arduino(int pin, int level, int pause) {
  digitalWrite(pin, level);
  delay(pause);
}

void melody(int start) {
  if (start == HIGH) {
    melody_on();
  } else {
    melody_off();
  }
}



//  мелодии включения
void melody_on() {
  // tone(pin, тон, длительность тона)
  const int pause_tone = 40;
  const int pause_led = 40;

  tone(indication.BUZZER, 2000, pause_tone);           // пик 1
  led_arduino(indication.LED_BOARD, HIGH, pause_led);  // вспышка 1 на плате вкл
  delay(10.0);

  led_arduino(indication.LED_BOARD, LOW, 200);
  tone(indication.BUZZER, 2000, pause_tone);  // пик 2

  led_arduino(indication.LED_BOARD, HIGH, pause_led);
  led_arduino(indication.LED_BOARD, LOW, 0);
}

// мелодии выключения
void melody_off() {
  tone(indication.BUZZER, 2000, 200.0);  // тон 2000 лучше слышно
  led_arduino(indication.LED_BOARD, HIGH, 400);
  led_arduino(indication.LED_BOARD, LOW, 0);
}

//мелодия предупреждения о мокрой кнопке
void rain_gardian_sound(int pause) {
  tone(indication.BUZZER, 2000, pause);
  delay(200);
  noTone(indication.BUZZER);
}
