/*
Скетч для эмуляции универсального hid - совместимого игрового контроллера.
На данный момент поддерживает только 5 каналов, используются прерывания для считывания.

Подключение:
0-ось Х
1-ось У 
2-газ
3-руль
7-кнопка 1

Поддерживаемые платы: Arduino leonardo,micro

Используемая библиотека для эмуляции джойстика:
https://github.com/MHeironimus/ArduinoJoystickLibrary
*/
#define LOWLEVEl 1000          //Низкий уровень с пульта, ms
#define HEIGHLEVEl 2000        //Высокий уровень с пульта, ms
#include <Arduino.h> //Если прогаете через arduino ide, то можно закомментить эту стоку
#include <Servo.h>
#include <Joystick.h>

unsigned long ch[8]; //Массив c значениями с пульта

void all_intterrupt(int channel, int pin)
{
    unsigned long mic = micros();
    channel = channel - 1;
    static unsigned long chHIGH[8];

    int buf = digitalRead(pin);
    if (buf == HIGH)
    {
        chHIGH[channel] = mic;
    }
    else if (mic > chHIGH[channel])
    {
        ch[channel] = mic - chHIGH[channel];
    }
}
void channel_1()
{
    all_intterrupt(1, 3);
}
void channel_2()
{
    all_intterrupt(2, 2);
}
void channel_3()
{
    all_intterrupt(3, 0);
}
void channel_4()
{
    all_intterrupt(4, 1);
}
void channel_5()
{
    all_intterrupt(5, 7);
}
void interrupt_on()
{
    attachInterrupt(0, channel_1, CHANGE);
    attachInterrupt(1, channel_2, CHANGE);
    attachInterrupt(2, channel_3, CHANGE);
    attachInterrupt(3, channel_4, CHANGE);
    attachInterrupt(4, channel_5, CHANGE);
}
void init_pins()
{
    pinMode(3, INPUT);
    pinMode(0, INPUT);
    pinMode(1, INPUT);
    pinMode(2, INPUT);
    pinMode(7, INPUT);
}
void joystick_x(int x)
{
    x = map(x, LOWLEVEl, HEIGHLEVEl, -127, 127);
    x = constrain(x, -127, 127);
    Joystick.setXAxis(x);
}
void joystick_y(int x)
{
    x = map(x, LOWLEVEl, HEIGHLEVEl, -127, 127);
    x = constrain(x, -127, 127);
    Joystick.setYAxis(x);
}
void joystick_gaz(int x)
{
    x = map(x, LOWLEVEl, HEIGHLEVEl, 0, 255);
    x = constrain(x, 0, 255);
    Joystick.setThrottle(x);
}
void joystick_rudder(int x)
{
    x = map(x, LOWLEVEl, HEIGHLEVEl, 0, 255);
    x = constrain(x, 0, 255);
    Joystick.setRudder(x);
}
void joystick_button1(int button)
{
    if (button > 1500)
    {
        Joystick.pressButton(0);
    }
    else
    {
        Joystick.releaseButton(0);
    }
}
void joystick()
{
    static long previousMillis = 0; // храним время последнего переключения
    static long interval = 20;      // интервал между отправкой значений джойстика на комп

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis > interval)
    {
        previousMillis = currentMillis;

        joystick_x(ch[2]);
        joystick_y(ch[3]);
        joystick_gaz(ch[1]);
        joystick_rudder(ch[0]);
        joystick_button1(ch[4]);
        Joystick.sendState();
    }
}
void debug() // для отладки считываемых значений с джойстика
{
    for (int i = 0; i < 8; i++)
    {
        Serial.print(ch[i]);
        Serial.print("  ");
    }
    Serial.println(" ");
    delay(300);
}
void setup()
{
    interrupt_on();
    Joystick.begin(false);
   // Serial.begin(9600);
}
void loop()
{
    //   debug();
    joystick();
}

