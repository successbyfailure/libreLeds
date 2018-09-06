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

#include "nLedBar.h"


baseNode*     node;
spifsStorage* s;

void ledsOff()
{
  node->lController()->off();
}

void artAddressReceived() {

  Serial.println("Arnet settings received, u: "+String(node->lController()->artnetInterface()->universe())+" id:"+String(node->lController()->artnetInterface()->longName()));
}
/*
  artIpProg callback allows storing of config information
  cmd field bit 7 indicates that settings should be programmed
*/
void artIpProgReceived(uint8_t cmd, IPAddress addr, IPAddress subnet)
{
 Serial.println("Artnet IP recv");
}

void launchConfigPortal(storage* s)
{
  Serial.println("Launching config portal...");

  int pin = String(s->getNodeConfig(STATUS_PIN)).toInt();
  pinMode(pin, OUTPUT);
  analogWrite(pin, 128);
  WiFiManager wifiManager;
  wifiManager.setTimeout(120);

  WiFiManagerParameter l          ("<p></p>");
  WiFiManagerParameter t0         ("nodeID:");
  WiFiManagerParameter nodeID     ("ni"   , "n_i"   ,String(s->getNodeConfig(NODE_ID)).c_str() , 40);

  WiFiManagerParameter t1         ("nodeType:");
  WiFiManagerParameter nodeType   ("nT" , "n_t"     ,String(s->getNodeConfig(NODE_TYPE)).c_str() , 40);

  WiFiManagerParameter t2         ("wifiMode:");
  WiFiManagerParameter wmode      ("wm", "w_m"      ,String(s->getNodeConfig(WIFI_MODE)).c_str() , 40);

  WiFiManagerParameter t3         ("MQTT Enabled:");
  WiFiManagerParameter mqttEn     ("mqEn", "mq_en"  ,String(s->getNodeConfig(MQTT_ENABLED)).c_str() , 40);

  WiFiManagerParameter t4         ("MQTT_IP:");
  WiFiManagerParameter mqttIP     ("mqIP", "mq_ip"  ,String(s->getNodeConfig(MQTT_SERVER)).c_str() , 40);

  WiFiManagerParameter t5         ("ArtNet: Enabled");
  WiFiManagerParameter artEn      ("aEn", "a_en"    ,String(s->getLedConfig(ARTNET_ENABLED)).c_str() , 40);

  WiFiManagerParameter t6         ("Universe");
  WiFiManagerParameter artUn      ("aU", "a_u"      ,String(s->getLedConfig(ARTNET_UNIVERSE)).c_str() , 40);

  WiFiManagerParameter t7         ("sACN");
  WiFiManagerParameter sacnEn     ("sEn", "s_en"    ,String(s->getLedConfig(SACN_ENABLED)).c_str() , 40);

  WiFiManagerParameter t8         ("Universe");
  WiFiManagerParameter sacnUn     ("sU", "s_u"      ,String(s->getLedConfig(SACN_UNIVERSE)).c_str() , 40);

  WiFiManagerParameter t9         ("LedType");
  WiFiManagerParameter ledType    ("ledType", "lt"  ,String(s->getLedConfig(LED_HW)).c_str() , 40);

  WiFiManagerParameter t10         ("ledCount");
  WiFiManagerParameter ledCount   ("ledC", "lc"     ,String(s->getLedConfig(LED_COUNT)).c_str() , 40);

  WiFiManagerParameter t11         ("ledMaxP");
  WiFiManagerParameter bright      ("ledP", "lp"    ,String(s->getLedConfig(LED_MAXBRIGHT)).c_str() , 40);

  wifiManager.addParameter(&l);
  wifiManager.addParameter(&t0);
  wifiManager.addParameter(&nodeID);
  wifiManager.addParameter(&l);
  wifiManager.addParameter(&t1);
  wifiManager.addParameter(&nodeType);
  wifiManager.addParameter(&l);
  wifiManager.addParameter(&t2);
  wifiManager.addParameter(&wmode);
  wifiManager.addParameter(&l);
  wifiManager.addParameter(&t3);
  wifiManager.addParameter(&mqttEn);
  wifiManager.addParameter(&t4);
  wifiManager.addParameter(&mqttIP);
  wifiManager.addParameter(&l);
  wifiManager.addParameter(&t5);
  wifiManager.addParameter(&artEn);
  wifiManager.addParameter(&t6);
  wifiManager.addParameter(&artUn);
  wifiManager.addParameter(&l);
  wifiManager.addParameter(&t7);
  wifiManager.addParameter(&sacnEn);
  wifiManager.addParameter(&t8);
  wifiManager.addParameter(&sacnUn);
  wifiManager.addParameter(&l);
  wifiManager.addParameter(&t9);
  wifiManager.addParameter(&ledType);
  wifiManager.addParameter(&t10);
  wifiManager.addParameter(&ledCount);
  wifiManager.addParameter(&t11);
  wifiManager.addParameter(&bright);


  if (!wifiManager.startConfigPortal(String(s->getNodeConfig(NODE_ID)).c_str(),String(WIFI_PWD).c_str()))
  {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.reset();
    delay(8000);
  }
  else
  {
    s->setNodeConfig(NODE_ID        , nodeID.getValue());
    s->setNodeConfig(NODE_TYPE      , nodeType.getValue());
    s->setNodeConfig(MQTT_ENABLED   , mqttEn.getValue());
    s->setNodeConfig(MQTT_SERVER    , mqttIP.getValue());
    s->setLedConfig(ARTNET_ENABLED , artEn.getValue());
    s->setLedConfig(ARTNET_UNIVERSE, artUn.getValue());
    s->setLedConfig(SACN_ENABLED   , sacnEn.getValue());
    s->setLedConfig(SACN_UNIVERSE  , sacnUn.getValue());
    s->setLedConfig(LED_HW         , ledType.getValue());
    s->setLedConfig(LED_COUNT      , ledCount.getValue());
    s->setLedConfig(LED_MAXBRIGHT  , bright.getValue());
    s->dumpConfig();
  }

}

bool enterConfigMode(storage* s)
{
  String cMode = s->getNodeConfig(CONFIG_MODE);
  Serial.println(String("Config mode: ")+cMode);
  if(cMode == ON_BOOT_ERROR)
  {
    int bootError = String(s->getNodeConfig(BERROR)).toInt();
    bootError++;
    if(bootError >= String(s->getNodeConfig(MAX_BERROR)).toInt())
    {
      s->setNodeConfig(BERROR,String(0));
      return true;
    }
    else
      s->setNodeConfig(BERROR,String(bootError));
  }
  else if(cMode == ON_PIN)
  {
    uint8_t pin = String(s->getNodeConfig(CONFIG_PIN)).toInt();
    pinMode(pin,INPUT);
    delay(50);
    if(digitalRead(pin) == HIGH)
    {
      return true;
    }
  }
  return false;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\nStart!");
  //delay(6000);//Delay para darle tiempo al atom para abrir el terminal serie
  s = new spifsStorage();
  //s->initConfig();
  //s->setKeyValue(CONFIG_MODE,ON_BOOT_ERROR);
  //s->setKeyValue("lr","5");
  //s->setKeyValue("ld","0");
  //s->setKeyValue("t","ledBar");
  //s->setKeyValue("lc","768");


  if(enterConfigMode(s))
  {
    launchConfigPortal(s);
  }

  String nodeT = s->getNodeConfig(NODE_TYPE);

  if(nodeT == "ledBar")
  {
    node = new ledBarNode(s);
  }
  else
  {
    node = new baseNode(s);
  }
  node->setup();
}

void loop()
{
    node->update();
}
