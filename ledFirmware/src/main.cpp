#include <Arduino.h>

//Defines y hacks
#define ADC_UNDERVOLT
#ifdef ADC_UNDERVOLT
  ADC_MODE(ADC_VCC)
#endif
//FASTLED
#define LED_PIN   D8
#define LED_CLOCK D7
//#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0

//librerias
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <EasyNTPClient.h>
#include <WiFiManager.h>
#include "FastLED.h"


#include <LXWiFiArtNet.h>
#include <LXWiFiSACN.h>

#include <PubSubClient.h>
//
#include "baseNode.h"


baseNode*     node;
spifsStorage* s;

void artAddressReceived() {

  Serial.println("Arnet settings rreceived, u: "+String(node->lController()->artnetInterface()->universe())+" id:"+String(node->lController()->artnetInterface()->longName()));
}
/*
  artIpProg callback allows storing of config information
  cmd field bit 7 indicates that settings should be programmed
*/
void artIpProgReceived(uint8_t cmd, IPAddress addr, IPAddress subnet)
{
 Serial.println("Artnet IP recv");
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\nStart!");
  //delay(500);//Delay para darle tiempo al atom para abrir el terminal serie
  s = new spifsStorage();
  //s->initConfig();
  //s->setKeyValue("lm","100");
  //s->setKeyValue("lr","5");
  //s->setKeyValue("ld","0");
  node = new baseNode(s);
  node->setup();
}

void loop()
{
    node->update();
}
