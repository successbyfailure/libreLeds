#ifndef QTCOMPAT_H
#define QTCOMPAT_H

#ifndef ESP8266
#ifndef __AVR_ATmega2560__
//QT creator compatibilidad
#include <QObject>
#include <vector>
typedef quint16 uint16_t;
typedef quint8  uint8_t;

#define A0 0
#define D0 0
#define D1 0
#define D2 0
#define D3 0
#define D4 0
#define D5 0
#define D6 0
#define D7 0
#define D8 0

void yield();
void delay(int ms);
void pinMode(int pin, int mode);
int  digitalRead(int pin);
int  analogRead(int pin);
void digitalWrite(int pin, bool state);
void analogWrite(int pin, int pwm);

class WiFiClient
{
public:
    WiFiClient();

};

class String
{
public:
    String();
    String(char*);
    void charAt(int i);
    void endsWith(String);
    void equals(String);
    void indexOf(String);
    int length();
    void remove();
    bool startsWith();
    String substring(int start, int end);
    void toCharArray(char* c, int count);
    void toInt();
    void toFloat();
    String toLowerCase();
    String toUpperCase();
    String trim();
};

class Serial
{
public:
    Serial();
    void begin(int baud);
    char read();
    void print();
    void println();
};


#endif
#endif


#endif // QTCOMPAT_H

