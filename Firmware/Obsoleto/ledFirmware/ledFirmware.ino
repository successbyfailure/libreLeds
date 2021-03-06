#include "qtcompat.h"

#include <gamma.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include <Wire.h>


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
#include "ledcontroller.h"
#include "protocol.h"

//Interrupcion para contar tiempo
os_timer_t  myTimer;
uint16_t lastCycleMS = 0;
uint16_t thisCycleMS = 0;
uint16_t aliveTimer  = 0;
uint16_t lastFreeMem = 0;
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

#define HIGHFREQCYCLE   0
#define MEDFREQCYCLE    25
#define LOWFREQCYCLE    200

void timerCallback(void *pArg)
{
      thisCycleMS ++;
}


EEPROMStorage myEEPROM;
basicSettings* settingsBasic;
extraSettings* settingsExtra;

ledController myLedController(&myEEPROM);


//Artnet
ArtnetWifi artnet;
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
    myLedController.onDmxFrame(universe,  length,  sequence, data);
    artnetPackets++;
    yield();
}

void timer_init() {
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

      os_timer_setfn(&myTimer, timerCallback, NULL);

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

      os_timer_arm(&myTimer, 1, true);
}

void imAlive()
{

    if(aliveTimer < myEEPROM.getBasicSettings().watchdogTime) return;

    digitalWrite(myEEPROM.getExtraSettings().alivePin, !digitalRead(myEEPROM.getExtraSettings().alivePin));

    Serial.print("ImAlive! - ");Serial.print(aliveTimer); Serial.print("ms");
    aliveTimer = 0;
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
    lastFreeMem     = system_get_free_heap_size();
    if(lastFreeMem < memoryShortageThreshold) memoryShortageHappend = true;

    delay(myEEPROM.getBasicSettings().delayTime);
    yield();

    lastCycleMS     = thisCycleMS;
    thisCycleMS     = 0;
    aliveTimer     += lastCycleMS;
    hfCounter      += lastCycleMS;
    mfCounter      += lastCycleMS;
    lfCounter      += lastCycleMS;
}

void setup()
{
//    Serial.begin(115200);
    timer_init();
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

    myLedController.setup();
    yield();

    Serial.println("Init wifi...");
    wifi_init(&myEEPROM);

    if(settingsExtra->artNetEnabled)
    {
        Serial.println("Loading artnet module...");
        artnet.begin();
        artnet.setArtDmxCallback(onDmxFrame);
    }

    yield();

    myLedController.getledGadget()->setAnimation(animationVUMeter);
    //myLedController.getledGadget()->strobe(50);

    Serial.print("\ninit done! took :");Serial.print(thisCycleMS);Serial.println("ms\n");
    thisCycleMS = 0;
}

void loop()
{

    if(lfCounter > LOWFREQCYCLE)
    {//Bucle de baja frecuencia
        myLedController.lfUpdate();
        if(myEEPROM.getBasicSettings().httpUpdaterEnabled)updateServer.handleClient();
        lfLoops++;
        lfCounter = 0;
    }

    if(mfCounter > MEDFREQCYCLE)
    {//Bucle de media frecuencia
        myLedController.mfUpdate();
        mfLoops++;
        mfCounter = 0;
    }

    if(hfCounter > HIGHFREQCYCLE)
    {//Bucle de alta frecuencia
        if(myEEPROM.getExtraSettings().artNetEnabled)artnet.read();
        myLedController.hfUpdate();
        hfLoops++;
        hfCounter = 0;
    }

    manageTimers();
    imAlive();
}
