#ifndef BASENODE
#define BASENODE


//Librerias
#include "aux.h"
#include "storage.h"
#include "ledController.h"

//

extern void ledsOff();

class baseNode
{
public:
baseNode(storage* s) :_storage(s),_ledController(s),
                      _ntpClient(_ntpUdp, "es.pool.ntp.org")
{
  _lastWifiStatus   = WiFi.status();
  _firstTimeConnect = true;
}

void setup()
{
  uint16_t now        = millis();
  _lastSensorLoopTime = now;
  _lastNodeLoopTime   = now;
  _lastLedLoopTime    = now;
  _lastOtherLoopTime  = now;

  reloadConfig  ();
  _ntpUdp.begin (123);
  pinMode       (_statusLedPin, OUTPUT);
  digitalWrite  (_statusLedPin,HIGH);
  setupNode();

  WiFi.persistent       (true);
  WiFi.mode             (WIFI_STA);
  WiFi.setOutputPower   (0);
  WiFi.setSleepMode     (WIFI_NONE_SLEEP);
  WiFi.setAutoConnect   (true);
  WiFi.setAutoReconnect (true);
  WiFi.hostname         (_id);

  WiFi.begin("localhost","aceptolosterminos");

  ArduinoOTA.setHostname(_id.c_str());
  //ArduinoOTA.setPassword("clubmate");
  ArduinoOTA.onStart([]() {
    ledsOff();
  });
  ArduinoOTA.begin();

  yield();
  Serial.println(String(F("\n\n\n ...OK! Setup took "))+String(millis())+String(F("ms")));
  imAlive();
}

void reloadConfig()
{
  _id               = _storage->getKeyValue(NODE_ID);
  _nodeType         = _storage->getKeyValue(NODE_TYPE);
  _statusLedPin     = _storage->getKeyValue(STATUS_PIN    ).toInt();
  _sensorLoopTimer  = _storage->getKeyValue(SENSOR_REFRESH).toInt();
  _nodeLoopTimer    = _storage->getKeyValue(NODE_REFRESH  ).toInt();
  _ledLoopTimer     = _storage->getKeyValue(LED_REFRESH   ).toInt();
  _otherLoopTimer   = _storage->getKeyValue(OTHER_REFRESH ).toInt();
  _loopDelay        = _storage->getKeyValue(LOOP_DELAY    ).toInt();
  _aliveTimer       = _storage->getKeyValue(WD_REFRESH    ).toInt();

  Serial.println(String(F("\nConfig loaded - id:"))+_id+ String(F(" nodeType:"))+_nodeType);
  Serial.println(String(F("Status pin:"))+String(_statusLedPin)+String(F(" delay:"))+String(_loopDelay)+String(F(" alive:"))+String(_aliveTimer));
}

void update()
{
  uint32_t loopStart = millis();

  checkConnectivity();
  handleLoops();

  uint32_t sysStart = millis();
  if( (millis() - _lastAliveTime) > _aliveTimer)
  {
    imAlive();
  }

  if(_statusLedDecay > 0)
  {
    _statusLedDecay -= _lastLoopMS;
    if(_statusLedDecay <= 0)
      digitalWrite(_statusLedPin,HIGH);
  }

  yield();
  _sysTime += millis() - sysStart;

  uint32_t sleepTime = millis();
  if(_loopDelay > 0)
    delay(_loopDelay);
  else
  {
    uint32_t now = millis();
    //cuanto queda hasta la siguente cosa a hacer?
      int timeToUpdateSensors = _lastSensorLoopTime - now + _sensorLoopTimer;
      int timeToUpdateLeds    = _lastLedLoopTime    - now + _ledLoopTimer;
      int timeToUpdateNode    = _lastNodeLoopTime   - now + _nodeLoopTimer;
      int timeToUpdateOther   = _lastOtherLoopTime  - now + _otherLoopTimer;

      int timeToNextThing     = timeToUpdateOther;
      if( timeToUpdateLeds    < timeToNextThing) timeToNextThing = timeToUpdateLeds;
      if( timeToUpdateNode    < timeToNextThing) timeToNextThing = timeToUpdateNode;
      if( timeToUpdateSensors < timeToNextThing) timeToNextThing = timeToUpdateSensors;
      if(timeToNextThing > 0)
        delay(timeToNextThing);
  }

  uint32_t now  = millis();
  _lastLoopTime = now;
  _sleptTime   += now - sleepTime;
  _lastLoopMS   =_lastLoopTime - loopStart;
}

void handleLoops()
{
  //gestion de loops
  if(_lastLoopTime > millis())
  {//el contador ha desbordado
    Serial.println("reset counters");
    _lastSensorLoopTime = 0;
    _lastNodeLoopTime   = 0;
    _lastLedLoopTime    = 0;
    _lastOtherLoopTime  = 0;
    _lastAliveTime      = 0;
  }
  _loops++;
  uint32_t now;
  now = millis();
  //Toca actualizar los sensores?
  if((now - _lastSensorLoopTime) > _sensorLoopTimer)
  {
    uint32_t startTime = millis();
    sensorLoop();
    _sensorLoops++;
    now = millis();
    _lastSensorLoopMS = now - startTime;
    _lastSensorLoopTime = now;
  }
  now = millis();
  //Toca actualizar los leds?
  if((now - _lastLedLoopTime) > _ledLoopTimer)
  {
    uint32_t startTime = millis();
    ledLoop();
    _ledLoops++;
    now = millis();
    _lastLedLoopMS = now - startTime;
    _lastLedLoopTime = now;
  }
  now = millis();
  //Toca actualizar el nodo?
  if((now - _lastNodeLoopTime) > _nodeLoopTimer)
  {//toca actualizar el nodo
    uint32_t startTime = millis();
    nodeLoop();
    _nodeLoops++;
    now = millis();
    _lastNodeLoopMS = now - startTime;
    _lastNodeLoopTime = now;
  }
  now = millis();
  if((now - _lastOtherLoopTime) > _otherLoopTimer)
  {//toca actualizar otras cosas de menos prioridad
    uint32_t startTime = millis();
    slowLoop();
    _otherLoops++;
    now = millis();
    _lastOtherLoopMS = now - startTime;
    _lastOtherLoopTime = now;
  }
}

virtual void ledLoop()
{
  _ledController.update();
}

virtual void slowLoop()
{
  ArduinoOTA.handle();
}

void imAlive()
{
  _lastAliveTime = millis();
  _statusLedDecay = 500;
  digitalWrite(_statusLedPin,LOW);

  uint16_t loops        = _loops        / (_aliveTimer/1000.0);
  uint16_t ledLoops     = _ledLoops     / (_aliveTimer/1000.0);
  uint16_t nodeLoops    = _nodeLoops    / (_aliveTimer/1000.0);
  uint16_t sensorLoops  = _sensorLoops  / (_aliveTimer/1000.0);
  uint16_t otherLoops   = _otherLoops   / (_aliveTimer/1000.0);
  _loops = 0, _ledLoops = 0, _nodeLoops = 0, _sensorLoops = 0, _otherLoops = 0;

  uint16_t sysTime       = _sysTime     / (_aliveTimer/1000.0);
  uint16_t sleptTime     = _sleptTime   / (_aliveTimer/1000.0);
  _sysTime = 0, _sleptTime = 0;

  uint16_t mqttPackets   = _mqttPackets  / (_aliveTimer/1000.0);
  _mqttPackets = 0;

  uint16_t artnetPackets = _ledController.getArtnetrx() / (_aliveTimer/1000.0);
  uint16_t sacnPackets   = _ledController.getsACNrx()   / (_aliveTimer/1000.0);
  uint16_t fps           = _ledController.getFrames()   / (_aliveTimer/1000.0);

  Serial.println(String(F("\n*Im alive!")));
  Serial.println(String(F("|-Uptime: ")) + String(millis()/1000.0)+"s");// - Epoch:" + String(_ntpClient.getUnixTime()));
  Serial.println(String(F("|-Node ID: ")) + _id + String(F("  - NodeType: ")) + _nodeType);
  Serial.println(String(F("|-Wifi ESSID: "))+String(WiFi.SSID())+String(F(" Status: ")) + String(WiFi.status()) + String(F(" Signal:")) + String(WiFi.RSSI()) + String(F("dbm  IP : ")) + localIP());
  Serial.println(String(F("|-Leds:")) + String(_ledController.ledCount()) + String(F(" MaxPower:")) +String(_ledController.maxPower())+ String(F(" Bright::")) +String(_ledController.brightness())+ String(F(" UnderVoltDimmer:")) +String(_ledController.undervoltDimmer()));
  Serial.println(String(F("|-Led Frames/s: ")) + String(fps));
  Serial.println(String(F("|")));
  Serial.println(String(F("|-VCC:")) + String(ESP.getVcc()/1000.0) + String(F(" - Freemem:")) +formatBytes(system_get_free_heap_size()));
  Serial.println(String(F("|")));
  yield();
  Serial.println(String(F("|-loop delay: "))+String(_loopDelay)+String(F("\tnodeLoopTimer: "))+String(_nodeLoopTimer)+String(F("\tledLoopTimer: "))+String(_ledLoopTimer)+String(F("\tsensorLoopTimer: "))+String(_sensorLoopTimer)+String(F("\totherLoopTimer: "))+String(_otherLoopTimer));
  Serial.println(String(F("|-Loops/s: "))+String(loops)+String(F("\tnodeLoops/s: "))+String(nodeLoops)+String(F("\tledLoops/s: "))+String(ledLoops)+String(F("\tsensorLoops/s: "))+String(sensorLoops)+String(F("\totherLoops/s: "))+String(otherLoops));
  Serial.println(String(F("|-Loop Time - last: "))+String(_lastLoopMS) +String(F("ms \tnode: "))+String(_lastNodeLoopMS)+String(F("ms \tled: "))+String(_lastLedLoopMS)+String(F("ms \tsensor: "))+String(_lastSensorLoopMS)+String(F("ms \tother: "))+String(_lastOtherLoopMS));
  Serial.println(String(F("| Sys:"))+String(sysTime) +String(F("ms \tslept: "))+String(sleptTime)+String(F("ms")));
  Serial.println(String(F("|")));
  Serial.println(String(F("|-RCV: mqtt/s: "))+String(mqttPackets)+String(F("\tartnet/s: "))+String(artnetPackets)+String(F("\tsacn/s: "))+String(sacnPackets));
  Serial.println(String(F("|___\n")));
  yield();
}


void rx()
{
  _statusLedDecay = 50;
  digitalWrite(_statusLedPin,LOW);
}

ledController* lController()
{
  return &_ledController;
}


virtual void setupNode()  {;}
virtual void sensorLoop() {;}
virtual void nodeLoop()   {;}


protected:
  String        _id;
  String        _nodeType;
  int           _statusLedPin;

  storage*      _storage;
  ledController _ledController;

  WiFiUDP       _ntpUdp;
  EasyNTPClient _ntpClient;

  uint8_t       _lastWifiStatus;
  uint8_t       _firstTimeConnect;
//temporizadores para stadisticas  y gestion de loops
  uint16_t _loops             = 0;
  uint16_t _sensorLoops       = 0;
  uint16_t _nodeLoops         = 0;
  uint16_t _ledLoops          = 0;
  uint16_t _otherLoops        = 0;

  uint16_t _sensorLoopTimer   = 10;
  uint16_t _nodeLoopTimer     = 100;
  uint16_t _ledLoopTimer      = 10;
  uint16_t _otherLoopTimer    = 250;
  uint16_t _aliveTimer        = 5000;

  uint32_t _lastLoopTime       = 0;
  uint32_t _lastSensorLoopTime = 0;
  uint32_t _lastNodeLoopTime   = 0;
  uint32_t _lastLedLoopTime    = 0;
  uint32_t _lastOtherLoopTime  = 0;
  uint32_t _lastAliveTime      = 0;

  uint16_t _lastSensorLoopMS   = 0;
  uint16_t _lastLedLoopMS      = 0;
  uint16_t _lastNodeLoopMS     = 0;
  uint16_t _lastOtherLoopMS    = 0;
  uint16_t _lastLoopMS         = 0;

  uint16_t _loopDelay         = 5;
  uint16_t _sysTime           = 0;
  uint16_t _sleptTime         = 0;

  int16_t _statusLedDecay     = 0;


  uint16_t _mqttPackets       = 0;


  void wifiDisconnected()
  {
    Serial.println(String(F("Wifi Disconnected!")));
  }
  void wifiConnected()
  {
    Serial.println(String(F("Wifi Connected!")));
    if(_firstTimeConnect)
      firstTimeConnect();
  }
  void firstTimeConnect()
  {
      _firstTimeConnect = false;
      Serial.println(String(F("Launching services...")));
      _ledController.initDMX();
      if(MDNS.begin(_id.c_str()))
      {
          Serial.println(String(F("MDNS started, name:"))+_id);
          MDNS.addService("http", "tcp", 80);
      }
      else
      {
          Serial.println(String(F("Failed to start MDNS")));
      }

  }

  void checkConnectivity()
  {
    uint8_t s = WiFi.status();
    if(WiFi.status() != _lastWifiStatus)
    {
      _lastWifiStatus = s;
      if(s == 3)
      {
        wifiConnected();
      }
      else
      {
        wifiDisconnected();
      }
    }
  }
};


#endif
