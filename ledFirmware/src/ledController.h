#ifndef LEDCONTROLLER
#define LEDCONTROLLER

#include "storage.h"

extern void artAddressReceived();
extern void artIpProgReceived(unsigned char, IPAddress, IPAddress);

class ledController{
public:
    ledController(storage* s)
    {
      _storage        = s;
      _artnetRX       = 0;
      _sacnRX         = 0;
      _dmxSetup       = false;
      _bytesPerPixel  = 3;
      _frames         = 0;
      _idleFrames     = 0;
    }

    void setup()
    {
      String ledhwType   = _storage->getKeyValue(LED_HW);
      _ledCount          = _storage->getKeyValue(LED_COUNT    ).toInt();
      _maxBright         = _storage->getKeyValue(LED_MAXBRIGHT).toInt();
      _brightness        = _storage->getKeyValue(LED_BRIGHT   ).toFloat();
      _statusLedPin      = _storage->getKeyValue(STATUS_PIN   ).toInt();
      _underVoltProtect  = _storage->getKeyValue(UNDERVOLT_PROTECT) == "true";
      _ledArray          =  new CRGB[_ledCount];


      Serial.println(String(F("Setting up led hardware... "))+ledhwType);
      if      (ledhwType == "ws2812Strip")
      {
        Serial.print(String(F("Neopixel")));
        LEDS.addLeds<NEOPIXEL,LED_PIN>(_ledArray, _ledCount);
      }
      else if (ledhwType == "ws2801strip")
      {
        Serial.print(String(F("WS2801")));
        LEDS.addLeds<WS2801,LED_PIN,LED_CLOCK,BGR>(_ledArray, _ledCount);
      }
      else if (ledhwType == "apa102strip")
      {
        Serial.print(String(F("APA102")));
        LEDS.addLeds<APA102, LED_PIN,LED_CLOCK,BGR>(_ledArray, _ledCount);
      }
      else
      {
        Serial.print(String(F("DefaultNeopixel")));
        LEDS.addLeds<NEOPIXEL,LED_PIN>(_ledArray, _ledCount);
      }
      Serial.println(String(F(" ledcount:"))+String(_ledCount)+" brightness: "+String(_brightness)+" * "+String(_maxBright)+" = "+String(_maxBright*_brightness));
      FastLED.setBrightness(_maxBright*_brightness);
      _pixelsPerUniverse = 512 / _bytesPerPixel;
      _maxDMXData        = (_ledCount*_bytesPerPixel)+(_ledCount/_pixelsPerUniverse);

      if(true)
      {
        calibratePower();
      }

      off();
      update();
      delay(10);
      _underVoltDimmer = 1.0;
      #ifdef ADC_UNDERVOLT
      _vccRef = ESP.getVcc()/1000.0;
      #endif

      //setColor(200, 200, 0);
    }

    uint16_t  ledCount()        {return _ledCount;}
    uint8_t   maxPower()        {return _maxBright;}
    float     brightness()      {return _brightness;}
    float     undervoltDimmer() {return _underVoltDimmer;}

    void setBrightness(float b)
    {
      _brightness = b;
      if(_brightness>1)
        _brightness = 1;
    }

    void setMaxPower(uint8_t b)
    {
      _maxBright = b;
    }

    void setColor(uint8_t r, uint8_t g, uint8_t b)
    {
        for(uint16_t i = 0 ; i < _ledCount ; i++)
          setPixelColor(i,r,g,b);
    }

    void setPixelColor(uint16_t index, uint8_t r, uint8_t g, uint8_t b,uint8_t w = 0)
    {
        if(index > _ledCount)
        {
          return;
        }
        _updated = true;
        _ledArray[index].r=r;
        _ledArray[index].g=g;
        _ledArray[index].b=b;
    }

    void off()
    {
      for(uint16_t i = 0 ; i < _ledCount ; i++)
        setPixelColor(i,0,0,0);
    }

    void update()
    {
      if(_underVoltProtect)
        checkVCC();

      if(_dmxSetup)
      {
        if(_sacnEnabled)
        {
          readsACN();
        }
        if(_artnetEnabled)
        {
          while (readArtnet()) {
            yield();
          }
        }
      }

      if(_updated)
      {
        show();
        _updated = false;
        if(_underVoltProtect)
          checkVCC();
      }
      else
      {
          _idleFrames++;
          if(_idleFrames > 100)
          {
            _idleFrames = 0;
            show();
            if(_underVoltProtect)
              checkVCC();
          }
      }

      uint32_t now = millis();
      if(_statusLedDecay > 0)
      {
        _statusLedDecay -= now - _lastUpdated;
        if(_statusLedDecay <=0)
          digitalWrite(_statusLedPin,HIGH);
      }
      _lastUpdated = now;
    }

    void show()
    {
      _frames++;
      FastLED.setBrightness(_maxBright*_brightness*_underVoltDimmer);
      FastLED.show();
    }

    uint16_t getsACNrx()
    {
      uint16_t r  = _sacnRX;
      _sacnRX     = 0;
      return r;
    }

    uint16_t getArtnetrx()
    {
      uint16_t r  = _artnetRX;
      _artnetRX   = 0;
      return r;
    }

    uint16_t getFrames()
    {
      uint16_t f  = _frames;
      _frames     = 0;
      return f;
    }

    LXWiFiArtNet* artnetInterface()
    {
      return _artNet0;
    }

    void initDMX()
    {
      _dmxSetup = true;
      if(_storage->getKeyValue(SACN_ENABLED) == "true")
      { //SACN
        _sacnEnabled    = true;
        _sacnUniverse   = _storage->getKeyValue(SACN_UNIVERSE).toInt();
        _sacnChannel    = _storage->getKeyValue(SACN_CHANNEL).toInt();
        _sACN0 = new LXWiFiSACN();
        _sACN0->setUniverse(_sacnUniverse);
        _sUDP.beginMulticast(WiFi.localIP(), IPAddress(239,255,0,_sacnUniverse), _sACN0->dmxPort());
        Serial.println("sACN enabled,channel:"+ String(_sacnChannel) + " universe: "+String(_sacnUniverse));
      }
      else
        _sacnEnabled = false;

      if(_storage->getKeyValue(ARTNET_ENABLED) == "true")
      { //ARTNET
        _artnetEnabled  = true;
        _artnetUniverse = _storage->getKeyValue(ARTNET_UNIVERSE).toInt();
        _artnetChannel  = _storage->getKeyValue(ARTNET_CHANNEL).toInt();
        Serial.println("Artnet enabled,channel:"+ String(_artnetChannel) + " universes: ");

        _artNet0 = new LXWiFiArtNet(WiFi.localIP(), WiFi.subnetMask(),&_dmxBuffer[0]);
        _artNet0->setUniverse(_artnetUniverse);  //setUniverse for LXArtNet class sets complete Port-Address
        Serial.println(_artnetUniverse);

        _artNet0->setArtAddressReceivedCallback(&artAddressReceived);
        _artNet0->setArtIpProgReceivedCallback(&artIpProgReceived);

        if( _maxDMXData >512)
        {
          _artNet1 = new LXWiFiArtNet(WiFi.localIP(), WiFi.subnetMask(),&_dmxBuffer[0]);
          _artNet1->setUniverse(_artnetUniverse+1);
          Serial.println(_artnetUniverse+1);
        }
        if( _maxDMXData >1024)
        {
          _artNet2 = new LXWiFiArtNet(WiFi.localIP(), WiFi.subnetMask(),&_dmxBuffer[0]);
          _artNet2->setUniverse(_artnetUniverse+2);
          Serial.println(_artnetUniverse+2);
        }
        if( _maxDMXData >1536)
        {
          _artNet3 = new LXWiFiArtNet(WiFi.localIP(), WiFi.subnetMask(),&_dmxBuffer[0]);
          _artNet3->setUniverse(_artnetUniverse+3);
          Serial.println(_artnetUniverse+3);
        }
        if( _maxDMXData >2048)
        {
          _artNet4 = new LXWiFiArtNet(WiFi.localIP(), WiFi.subnetMask(),&_dmxBuffer[0]);
          _artNet4->setUniverse(_artnetUniverse+4);
          Serial.println(_artnetUniverse+4);
        }


        if( _storage->getKeyValue(ARTNET_ANNOUNCE) == "true")
        {
          Serial.println(F("Artnet poll enabled"));
          _aUDP.begin(_artNet0->dmxPort());
          String id = _storage->getKeyValue(NODE_ID);
          String id0 = id+"-0";
          strcpy(_artNet0->longName(), id0.c_str());
          _artNet0->send_art_poll_reply(&_aUDP);
          yield();

          if( _maxDMXData >512)
          {
            String id1 = id+"-1";
            strcpy(_artNet1->longName(), id1.c_str());
            _artNet1->send_art_poll_reply(&_aUDP);
            yield();
          }
          if( _maxDMXData >1024)
          {
            String id2 = id+"-2";
            strcpy(_artNet2->longName(), id2.c_str());
            _artNet2->send_art_poll_reply(&_aUDP);
            yield();
          }
          if( _maxDMXData >1536)
          {
            String id3 = id+"-3";
            strcpy(_artNet3->longName(), id3.c_str());
            _artNet3->send_art_poll_reply(&_aUDP);
            yield();
          }
          if( _maxDMXData >2048)
          {
            String id4 = id+"-4";
            strcpy(_artNet4->longName(), id4.c_str());
            _artNet4->send_art_poll_reply(&_aUDP);
            yield();
          }
        }
     }
     else
     {
      _artnetEnabled = false;
     }
    }

    void checkVCC()
  	{
      if(_maxBright < 3)
        return;

  		float currentVolts = ESP.getVcc()/1000.0;

      if(currentVolts > _vccRef)
        _vccRef = currentVolts;
      if(_underVoltDimmer > 1)
        _underVoltDimmer = 1;

      float undervolt = _vccRef - currentVolts;

  		if((undervolt < 0.1) && (_underVoltDimmer < 1.0))
  		{
        _underVoltDimmer += 0.00005;
  			//Serial.println("Voltage is good, rising factor: "+String(m_ledHardware->underVoltDimm())+" Result: "+ String(currentVolts));
  		}
      else if(undervolt >= 0.3)
      {
        _underVoltDimmer = 0.02;
  			show();
  		}
      else if(undervolt >= 0.1)
      {
        _underVoltDimmer -= undervolt/3;
        if(_underVoltDimmer < 0)
          _underVoltDimmer = 0.01;
  			show();
  			//Serial.println("Voltage is good, rising factor: "+String(m_ledHardware->underVoltDimm())+" Result: "+ String(currentVolts));
  		}
  	}

    void calibratePower()
    {
      #ifdef ADC_UNDERVOLT
      off();
      FastLED.show();
      delay(50);
      float vccRef = ESP.getVcc()/1000.0;
      for(int i = 0 ; i < 200 ; i++)
      {
        _maxBright = 200;
        setColor(255,255,255);
        FastLED.setBrightness(i);
        FastLED.show();
        delay(5);
        float volts = ESP.getVcc()/1000.0;
        if((vccRef - volts) > 0.1)
        {
          if(i < 3)
          _maxBright = 1;
        else
          _maxBright = i-2;
        break;
        }
      }
      FastLED.setBrightness(_maxBright);
      off();
      FastLED.show();
      delay(200);
      #endif
    }

protected:
  storage*  _storage;
  uint16_t  _ledCount;
  CRGB*     _ledArray;
  uint8_t   _maxBright;
  float     _brightness;
  bool      _underVoltProtect;
  float     _underVoltDimmer;
  float     _vccRef;
  uint8_t   _statusLedPin;


  int       _statusLedDecay;
  uint32_t  _lastUpdated;
  uint16_t  _frames;
  uint16_t  _idleFrames;
  bool      _updated;



//Artnet & sacn
  bool          _dmxSetup;
  uint8_t       _bytesPerPixel;
  uint8_t       _pixelsPerUniverse;
  bool          _sacnEnabled;
  uint16_t      _sacnUniverse;
  uint16_t      _sacnChannel;
  bool          _artnetEnabled;
  uint16_t      _artnetUniverse;
  uint16_t      _artnetChannel;
  uint8_t       _dmxBuffer[SACN_BUFFER_MAX];
  uint16_t      _maxDMXData;
  uint16_t      _artnetRX;
  uint16_t      _sacnRX;
  LXWiFiArtNet* _artNet0;
  LXWiFiArtNet* _artNet1;
  LXWiFiArtNet* _artNet2;
  LXWiFiArtNet* _artNet3;
  LXWiFiArtNet* _artNet4;
  LXWiFiSACN*   _sACN0;

  WiFiUDP _aUDP;
  WiFiUDP _sUDP;




  bool readsACN()
  {
    bool updated = false;
    if(_sACN0->readDMXPacket(&_sUDP) == RESULT_DMX_RECEIVED)
    {
      _sacnRX++;
      _statusLedDecay = 20;
      digitalWrite(_statusLedPin,LOW);
      uint16_t s_slots = _sACN0->numberOfSlots();
      for(int i = 1; i < s_slots ; i+=3)
      {
        if(i>_maxDMXData) break;
          setPixelColor(i/_bytesPerPixel, _sACN0->getSlot(i), _sACN0->getSlot(i+1), _sACN0->getSlot(i+2));
      }
      updated = true;
    }
    return updated;
  }

  bool readArtnet()
  {
    bool updated = false;
    uint16_t packetSize = _aUDP.parsePacket();
    while(packetSize)
    {
      _artnetRX++;
      _statusLedDecay = 20;
      digitalWrite(_statusLedPin,LOW);

      packetSize = _aUDP.read(_dmxBuffer, SACN_BUFFER_MAX);
    //U0
      if(_artNet0->readDMXPacketContents(&_aUDP, packetSize) == RESULT_DMX_RECEIVED)
      {
        for(int i = _artnetChannel; i < _artNet0->numberOfSlots() - 3 ; i+=_bytesPerPixel)
        {
          uint16_t p = i/_bytesPerPixel;
          setPixelColor(p, _artNet0->getSlot(i), _artNet0->getSlot(i+1), _artNet0->getSlot(i+2));
        }
        updated = true;
      }
    //U1
      else if((_maxDMXData>512) && (_artNet1->readDMXPacketContents(&_aUDP, packetSize) == RESULT_DMX_RECEIVED))
      {
        for(int i = 1; i < _artNet1->numberOfSlots() - 3 ; i+=_bytesPerPixel)
        {
          uint16_t p = (i/_bytesPerPixel) + _pixelsPerUniverse;
          setPixelColor(p, _artNet1->getSlot(i), _artNet1->getSlot(i+1), _artNet1->getSlot(i+2));
        }
        updated = true;
    //U2
    }
    else if((_maxDMXData>1024) && (_artNet2->readDMXPacketContents(&_aUDP, packetSize) == RESULT_DMX_RECEIVED))
    {
      for(int i = 1; i < _artNet2->numberOfSlots() - 3 ; i+=_bytesPerPixel)
      {
        uint16_t p = (i/_bytesPerPixel) + (_pixelsPerUniverse*2);
        setPixelColor(p, _artNet2->getSlot(i), _artNet2->getSlot(i+1), _artNet2->getSlot(i+2));
      }
      updated = true;
    //U3
    }
     else if((_maxDMXData>1536) && (_artNet3->readDMXPacketContents(&_aUDP, packetSize) == RESULT_DMX_RECEIVED))
      {
        for(int i = 1; i < _artNet3->numberOfSlots() - 3 ; i+=_bytesPerPixel)
        {
          uint16_t p = (i/_bytesPerPixel) + (_pixelsPerUniverse*3);
          setPixelColor(p, _artNet3->getSlot(i), _artNet3->getSlot(i+1), _artNet3->getSlot(i+2));
        }
        updated = true;
    //U4
    }
    else if((_maxDMXData>2048) && (_artNet4->readDMXPacketContents(&_aUDP, packetSize) == RESULT_DMX_RECEIVED))
     {
       for(int i = 1; i < _artNet4->numberOfSlots() - 3 ; i+=_bytesPerPixel)
       {
         uint16_t p = (i/_bytesPerPixel) + (_pixelsPerUniverse*4);
         setPixelColor(p, _artNet4->getSlot(i), _artNet4->getSlot(i+1), _artNet4->getSlot(i+2));
       }
       updated = true;
    }
    packetSize = _aUDP.parsePacket();
  }
  return updated;
 }
};

#endif // LEDCONTROLLER
