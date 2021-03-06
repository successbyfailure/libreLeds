#ifndef LEDCONTROLLER
#define LEDCONTROLLER

#include "qtcompat.h"

#include "eepromstorage.h"
#include "ledhardware.h"
#include "ledgadget.h"
#include "ledwheel.h"
#include "ledTree.h"
#include "libresaber.h"
#include "kineticWheels.h"
#include "vector"
#include "settings.h"
#include "ledgadgetclient.h"


class ledController{
public:
    ledController(settingsStorage* storage) :
        m_server(storage)
    {
        m_ledGadget = 0;
        m_settingsStorage = storage;
        extraSettings* es = &m_settingsStorage->getExtraSettings();
        startUniverse = es->startUniverse;
        Serial.print("Setting up led hardware, ");Serial.print(es->ledCount);Serial.println("leds");

        if      (es->ledhwType == hwWS2812Strip)
            m_ledHardware = new ws2812Strip(es);

        else if(es->ledhwType == hwWS2812RGWStrip)
            m_ledHardware = new ws2812RGBWStrip(es);

        else if (es->ledhwType == hwWS2812Matrix)
            m_ledHardware = new ws2812Matrix(es);
        
        else if (es->ledhwType == hwWS2801Strip)
            m_ledHardware = new ws2801Strip(es);
        
        else if (es->ledhwType == hwAPA102Strip)
            m_ledHardware = new apa102Strip(es);
        
        else if (es->ledhwType == hwAPA102Matrix)
            m_ledHardware = new apa102Matrix(es);
        //Setting ledgadget type
        if      (es->gadgetType == gadgetSpotLed)
            m_ledGadget = new spotLed   (es,m_ledHardware->getColorArray(),0,es->ledCount);
        else if (es->gadgetType == gadgetLedBar)
            m_ledGadget = new ledBar    (es,m_ledHardware->getColorArray(),0,es->ledCount,es->ledReversedOrder);
        else if (es->gadgetType == gadgetLedCircle)
            m_ledGadget = new ledCircle (es,m_ledHardware->getColorArray(),0,es->ledCount,es->ledReversedOrder);
        else if (es->gadgetType == gadgetLedWheel)
            m_ledGadget = new ledWheel  (es,m_ledHardware->getColorArray(),0,es->ledCount,es->ledReversedOrder);
        else if (es->gadgetType == gadgetledMatrix)
            m_ledGadget = new ledMatrix (es,(ledMatrixHW*)m_ledHardware   ,0,es->ledCount,es->ledReversedOrder);
        else if (es->gadgetType == gadgetledTree)
            m_ledGadget = new ledTree (es,m_ledHardware->getColorArray(),0,es->ledCount,es->ledReversedOrder);
        else if (es->gadgetType == gadgetLibreSaber)
            m_ledGadget = new libreSaber(es,m_ledHardware->getColorArray(),0,es->ledCount,es->ledReversedOrder);
        else if (es->gadgetType == gadgetKineticWheels)
            m_ledGadget = new kineticWheels(es,m_ledHardware->getColorArray(),0,es->ledCount,es->ledReversedOrder);
        else
            m_ledGadget = new ledBar    (es,m_ledHardware->getColorArray(),0,es->ledCount,es->ledReversedOrder);

        m_server.setLedGadget(m_ledGadget);;
    }

    ledGadget* getledGadget() {return m_ledGadget;}

    void setup()
    {
        m_ledHardware->setup();
        m_ledHardware->test();
        if(m_settingsStorage->getExtraSettings().artNetEnabled)
            initArtNet();

        m_server.begin();
    }

    void setBrightness(uint8_t b)
    {
        m_ledHardware->setBrightness(b);
    }

	void checkEnergy()
	{

		float currentVolts = ESP.getVcc()/1000.0;
		if(currentVolts >3.03 && (m_ledHardware->underVoltDimm() != 1.0))
		{
			m_ledHardware->underVoltDimmer(0.0001);
			m_ledHardware->refresh();
			//Serial.println("Voltage is good, rising factor: "+String(m_ledHardware->underVoltDimm())+" Result: "+ String(currentVolts));
		}
		
		if(currentVolts < 2.7)
		{
			//Serial.println("UnderVoltage: " + String(currentVolts) + " . Emegency undervolt!!!");
			m_ledHardware->underVoltDimmer(-0.99);
			m_ledHardware->refresh();
			//currentVolts = ESP.getVcc()/1000.0;
			//Serial.println("Factor: "+String(m_ledHardware->underVoltDimm())+" Result: "+ String(currentVolts));
		}
		else if(currentVolts < 2.9)
		{
			//Serial.println("UnderVoltage: " + String(currentVolts) + " . Reducing bright...");
			m_ledHardware->underVoltDimmer((currentVolts-3)*1.5);
			m_ledHardware->refresh();
			//currentVolts = ESP.getVcc()/1000.0;
			//Serial.println("Factor: "+String(m_ledHardware->underVoltDimm())+" Result: "+ String(currentVolts));
		}
		else if(currentVolts < 3)
		{
			//Serial.println("UnderVoltage: " + String(currentVolts) + " . Reducing bright...");
			m_ledHardware->underVoltDimmer((currentVolts-3)*0.1);
			m_ledHardware->refresh();
			//currentVolts = ESP.getVcc()/1000.0;
			//Serial.println("Factor: "+String(m_ledHardware->underVoltDimm())+" Result: "+ String(currentVolts));
		}
	}    
	
	
    void update()
    {
        hfUpdate();
        mfUpdate();
        lfUpdate();
    }

    void hfUpdate()
    {
        m_ledGadget->readSensors();
        checkEnergy();
    }

    void mfUpdate()
    {
        m_server.update();
        m_ledGadget->animate();
        m_ledHardware->refresh();

    }

    void lfUpdate()
    {
        //if(m_ledGadget->eq())
        //{
        //    sendVuLevels(m_ledGadget->eq()->vuLevel(),m_ledGadget->eq()->spectrum());
        //}
    }

    void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
    {
      sendFrame = 1;
	  //Serial.println("DMX: u: "+String(universe)+" l: "+String(length)+" seq:"+String(sequence));
      // set brightness of the whole strip
      //if (universe == 15)
      //{
      //  m_ledHardware->setBrightness(data[0]/255.0f);
      //  m_ledHardware->refresh();
      //}

      if(universe < startUniverse)
      {
          dmxBadFrames++;
          return;
      }

      m_ledGadget->setAnimation(animationNone);

      // Store which universe has got in
      if ((universe - startUniverse) < maxUniverses)
        universesReceived[universe - startUniverse] = 1;

//Esto no se que hace,
      for (int i = 0 ; i < maxUniverses ; i++)
      {
        if (universesReceived[i] == 0)
        {
          //Serial.println("Broke");
          sendFrame = 0;
          break;
        }
      }

      // read universe and put into the right part of the display buffer
      for (uint16_t i = 0; i < length / 3; i++)
      {
        uint16_t led = i + (universe - startUniverse) * (previousDataLength / 3);
        if(led > m_ledHardware->ledCount())
            dmxErrors++;
        else
        {
            m_ledHardware->setLedColor(led,CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]));
            dmxOK++;
        }
      }
      previousDataLength = length;

      if (sendFrame)
      {
        //m_ledHardware->refresh();
        // Reset universeReceived to 0
        memset(universesReceived, 0, maxUniverses);
      }
      dmxFrames++;
    }

    ledGadGetServer& server()    	{return m_server;}
	ledHardWare*     ledHW()        {return m_ledHardware;}

    void initAnimation()
    {
        m_ledHardware->initAnimation();
    }

    void sendPeak(float bpm = 0)
    {
        ledGadgetProtocolPacket p;
        p.cmd = msgPeak;
        p.floatVal0 = bpm;
        std::vector<protocolClient*> clients = m_server.getClients();
        for(int c = 0 ; c < clients.size() ; c++)
        {
            ledGadgetClient* lgc = (ledGadgetClient*) clients[c];
            lgc->sendLedGadgetProtocolPacket(p);
        }
    }


    void sendVuLevels(uint8_t vuLevel, uint8_t* spectrum)
    {
        ledGadgetProtocolPacket p;
        p.cmd = msgVuLevel;

        p.intVal0 = vuLevel;
        p.intVal1 = spectrum[0];
        p.intVal2 = spectrum[1];
        p.intVal3 = spectrum[2];
        p.intVal4 = spectrum[3];
        p.intVal5 = spectrum[4];
        p.intVal6 = spectrum[5];
        p.intVal0 = spectrum[6];

        std::vector<protocolClient*> clients = m_server.getClients();
        for(int c = 0 ; c < clients.size() ; c++)
        {
            ledGadgetClient* lgc = (ledGadgetClient*) clients[c];
            lgc->sendLedGadgetProtocolPacket(p);
        }
    }


    uint16_t getDmxFrames()
    {
        return dmxFrames;
    }

    
    uint16_t getBadDmxFrames()
    {
        return dmxBadFrames;
    }

    uint16_t getDmxErrors()
    {
        return dmxErrors;
    }

    uint16_t getDmxOK()
    {
        return dmxOK;
    }
    
    void resetDmxcounter()
    {
      dmxFrames = 0;
      dmxBadFrames = 0;
      dmxErrors = 0;
      dmxOK = 0;
    }
    
protected:
    ledHardWare*            m_ledHardware;
    ledGadget*              m_ledGadget;
    settingsStorage*        m_settingsStorage;
    ledGadGetServer         m_server;

    //artnet
    // Artnet settings
    bool  sendFrame;
    int   startUniverse;
    int   previousDataLength;
    int   numberOfChannels;
    int   maxUniverses;
    bool* universesReceived;
    uint16_t dmxFrames;
    uint16_t dmxBadFrames;
    uint16_t dmxErrors;
    uint16_t dmxOK;


    void initArtNet()
    {
        //artnet
        // Artnet settings
        sendFrame           = 1;
        startUniverse       = m_settingsStorage->getExtraSettings().startUniverse; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.
        previousDataLength  = 0;
        numberOfChannels    = m_settingsStorage->getExtraSettings().ledCount * 3; // Total number of channels you want to receive (1 led = 3 channels)
        maxUniverses        = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
		Serial.println("Init artnet, universe:"+String(startUniverse)+" Channels:"+String(numberOfChannels)+" Universes:"+String(maxUniverses));
        universesReceived   = new bool[maxUniverses];
        resetDmxcounter();
    }
};

#endif // LEDCONTROLLER

