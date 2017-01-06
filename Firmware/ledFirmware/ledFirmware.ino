#include "qtcompat.h"

//FASTLED
#define LED_PIN   D4
#define LED_CLOCK D3
#define FASTLED_ESP8266_RAW_PIN_ORDER


#include <gamma.h>
#include "FastLED.h"
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>


#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266mDNS.h>
  #include <ESP8266HTTPUpdateServer.h>
  #include <FS.h>
#endif

#include "vector"


#include <ArtnetWifi.h>

#include "espwifi.h"
#include "eepromstorage.h"
#include "protocol.h"
#include "ledcontroller.h"


//Interrupcion para contar tiempo
os_timer_t  myTimer;
uint16_t    timerInterval      = 0;

uint16_t lastCycleMS           = 0;
uint16_t thisCycleMS           = 0;
uint16_t timeSinceLastFrameMS  = 0;
uint16_t aliveTimer            = 0;
uint16_t lastFreeMem           = 0;

bool            memoryShortageHappend       = false;
const uint16_t  memoryShortageThreshold     = 15000;

uint16_t hfCounter = 0;
uint16_t mfCounter = 0;
uint16_t lfCounter = 0;

uint16_t hfLoops = 0;
uint16_t mfLoops = 0;
uint16_t lfLoops = 0;

uint16_t tcpPackets     = 0;
uint16_t artnetPackets  = 0;

EEPROMStorage myEEPROM;
basicSettings* settingsBasic;
extraSettings* settingsExtra;

ledController myLedController(&myEEPROM);

void timerCallback(void *pArg)
{
      thisCycleMS +=timerInterval;
      lastFreeMem     = system_get_free_heap_size();
      if(lastFreeMem < memoryShortageThreshold) memoryShortageHappend = true;
}

void initAnimationCallback(void *pArgs)
{
    myLedController.initAnimation();
    timerCallback(pArgs);
}


//Artnet
ArtnetWifi artnet;

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
    myLedController.onDmxFrame(universe,  length,  sequence, data);
    artnetPackets++;
    yield();
}

void onPeak(float bpm)
{
    myLedController.sendPeak(bpm);
}

void timer_init(uint16_t interval ,void (*fptr)(void *pArg)) {
 /*
  os_timer_setfn - Define a function to be called when the timer fires

void os_timer_setfn(
      os_timer_t *pTimer,
      os_timer_func_t *pFunction,
      void *pArg)

Define the callback function that will be called when the timer reaches zero. The pTimer parameters is a pointer to the timer control structure.

The pFunction parameters is a pointer to the callback function.

The pArg parameter is a value that will be passed into the called back function. The callback function should have the signature:
void (*functionName)(void *pArg)

The pArg parameter is the value registered with the callback function.
*/
    os_timer_disarm(&myTimer);
    timerInterval = interval;
      os_timer_setfn(&myTimer, fptr, NULL);

/*
      os_timer_arm -  Enable a millisecond granularity timer.

void os_timer_arm(
      os_timer_t *pTimer,
      uint32_t milliseconds,
      bool repeat)

Arm a timer such that is starts ticking and fires when the clock reaches zero.

The pTimer parameter is a pointed to a timer control structure.
The milliseconds parameter is the duration of the timer measured in milliseconds. The repeat parameter is whether or not the timer will restart once it has reached zero.

*/

      os_timer_arm(&myTimer, interval, true);
}

void imAlive()
{
    if(aliveTimer < myEEPROM.getBasicSettings().watchdogTime) return;

    digitalWrite(myEEPROM.getExtraSettings().alivePin, !digitalRead(myEEPROM.getExtraSettings().alivePin));

    Serial.print("ImAlive! - ");Serial.print(aliveTimer); Serial.print("ms");
    aliveTimer = 0;
    Serial.print("\tWifiStatus:");Serial.print(WiFi.status());
    Serial.print("\t IPaddr: ");
    if(wifiAPMode)
        Serial.print(WiFi.softAPIP());
    else
        Serial.print(WiFi.localIP());
    Serial.print("\tActiveClients:! - ");Serial.println(myLedController.server().getClients().size());

    Serial.print("delayTime: ");         Serial.print(myEEPROM.getBasicSettings().delayTime);
    Serial.print("\t- loopTime: ");      Serial.print(lastCycleMS);
    Serial.print("\t- Free: ");          Serial.print(lastFreeMem);
    Serial.print("\t - MemShortage: ");  Serial.println(memoryShortageHappend);

    float factor = 1000.0 / myEEPROM.getBasicSettings().watchdogTime;

    Serial.print("Cycles per second: hf:");Serial.print(hfLoops*factor);
    Serial.print("\tmf:");Serial.print(mfLoops*factor);
    Serial.print("\tlf:");Serial.println(lfLoops*factor);

    Serial.print("tcp packets/s:");Serial.print(tcpPackets*factor);
    Serial.print("\t artnet Packets/s:");Serial.println(artnetPackets*factor);

    Serial.println("....");
    hfLoops = 0;
    mfLoops = 0;
    lfLoops = 0;

    tcpPackets    = 0;
    artnetPackets = 0;
    yield();
}

void manageTimers()
{
    delay(myEEPROM.getBasicSettings().delayTime);
    yield();

    lastCycleMS           = thisCycleMS;
    thisCycleMS           = 0;
    timeSinceLastFrameMS += lastCycleMS;
    aliveTimer           += lastCycleMS;
    hfCounter            += lastCycleMS;
    mfCounter            += lastCycleMS;
    lfCounter            += lastCycleMS;
}

void setup()
{
    Serial.println("\n\ninit...");

    if(myEEPROM.hasBasicSettings())
        Serial.println("Using EEPROM settings");
    else
        Serial.println("Using hardcoded settings");
    settingsBasic = &myEEPROM.getBasicSettings();
    settingsExtra = &myEEPROM.getExtraSettings();

    Serial.println("Setting up hardware");
    pinMode(settingsExtra->alivePin,OUTPUT);
    digitalWrite(settingsExtra->alivePin,LOW);

    Serial.print("Init leds... count:");Serial.println(settingsExtra->ledCount);
    myLedController.setup();
    yield();

    timer_init(50,initAnimationCallback);

    Serial.println("Init wifi...");
    wifi_init(&myEEPROM);

    if(settingsExtra->artNetEnabled)
    {
        Serial.println("Loading artnet module...");
        artnet.begin();
        artnet.setArtDmxCallback(onDmxFrame);
    }

    myLedController.getledGadget()->setPeakCallback(onPeak);

    yield();

    //myLedController.getledGadget()->setAnimation(animationVUMeter);
    //myLedController.getledGadget()->strobe(50);
    timer_init(1,timerCallback);
    Serial.print("\ninit done! took :");Serial.print(thisCycleMS);Serial.println("ms\n");
    thisCycleMS = 0;

    if      (WiFi.status() == 3)
    {
        myLedController.server().connectMaster();
        myLedController.getledGadget()->green(100);
    }
    else if (WiFi.status() == 0)
        myLedController.getledGadget()->blue(100);
    else
        myLedController.getledGadget()->red(100);
    myLedController.getledGadget()->setNextAnimation(settingsExtra->defaultAnimation);
    myLedController.getledGadget()->fadeToNext();

    myLedController.update();
    imAlive();
}

void loop()
{

    if(lfCounter > settingsBasic->lowFreqCycleMs)
    {//Bucle de baja frecuencia
        myLedController.lfUpdate();
        if(myEEPROM.getBasicSettings().httpUpdaterEnabled)
            updateServer.handleClient();
        if(myEEPROM.getBasicSettings().httpServerEnabled)
            fileServer.handleClient();
        lfLoops++;
        lfCounter = 0;
    }

    if(mfCounter > settingsBasic->medFreqCycleMs)
    {//Bucle de media frecuencia
        myLedController.mfUpdate();
        mfLoops++;
        mfCounter = 0;
    }

    if(hfCounter > settingsBasic->highFreqCycleMs)
    {//Bucle de alta frecuencia
        if(myEEPROM.getExtraSettings().artNetEnabled)
            artnet.read();
        myLedController.hfUpdate();
        hfLoops++;
        hfCounter = 0;
    }

    manageTimers();
    imAlive();
}

