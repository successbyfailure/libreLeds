#ifndef LEDCONTROLLER
#define LEDCONTROLLER

#include "qtcompat.h"

#include "eepromstorage.h"
#include "ledhardware.h"
#include "ledgadget.h"
#include "ledwheel.h"
#include "vector"
#include "settings.h"
#include "ledgadgetclient.h"



//artnet
// Artnet settings
bool sendFrame = 1;
int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.
int previousDataLength = 0;
const int numberOfChannels = 100 * 3; // Total number of channels you want to receive (1 led = 3 channels)
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];

class ledController{
public:
    ledController(settingsStorage* storage) :
        m_server(storage)
    {
        m_ledGadget = 0;
        m_settingsStorage = storage;
        extraSettings es = m_settingsStorage->getExtraSettings();
        startUniverse = es.startUniverse;

        if      (es.ledhwType == hwWS2812Strip)
            m_ledHardware = new ws2812Strip(es.ledPin,es.ledCount,es.ledMaxBright);
        //else if (s.ledhwType == hwWS2812Matrix)
            //m_ledHardware = new ws2812Matrix();
        //else if (s.ledhwType == hwAPA102Strip)
            //m_ledHardware = new apa102Strip();

        std::vector<color*> pArray;
        for(int i = 0 ; i < m_ledHardware->colorArray()->size() ; i++)
            pArray.push_back(&m_ledHardware->colorArray()->at(i));

        if      (es.gadgetType == gadgetSpotLed)
            m_ledGadget = new spotLed(&es,pArray,0,es.ledCount);
        else if (es.gadgetType == gadgetLedBar)
            m_ledGadget = new ledBar(&es,pArray,0,es.ledCount,es.ledReversedOrder);
        else if (es.gadgetType == gadgetLedCircle)
            m_ledGadget = new ledCircle(&es,pArray,0,es.ledCount,es.ledReversedOrder);
        else if (es.gadgetType == gadgetLedWheel)
            m_ledGadget = new ledWheel(&es, pArray,0,es.ledCount,es.ledReversedOrder);
        else return;
        //else if (s.gadgetType == gadgetledMatrix)
            //m_ledGadgets.push_back(new ledMatrix());
        m_server.setLedGadget(m_ledGadget);
    }

    ledGadget* getledGadget() {return m_ledGadget;}

    void setup()
    {
        m_ledHardware->setup();
        m_ledHardware->test();
        m_server.begin();
    }

    void update()
    {
        hfUpdate();
        mfUpdate();
        lfUpdate();
    }

    void hfUpdate()
    {

        m_ledGadget->animate();
        m_ledHardware->refresh();
    }

    void mfUpdate()
    {
        m_server.update();
    }

    void lfUpdate()
    {

    }

    void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
    {
      m_ledGadget->setAnimation(animationNone);
      sendFrame = 1;
      // set brightness of the whole strip
      if (universe == 15)
      {
        m_ledHardware->setBrightness(data[0]/255.0f);
        m_ledHardware->refresh();
      }

      // Store which universe has got in
      if ((universe - startUniverse) < maxUniverses)
        universesReceived[universe - startUniverse] = 1;

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
      uint16_t ledCount = m_ledHardware->colorArray()->size();
      for (uint16_t i = 0; i < length / 3; i++)
      {
        uint16_t led = i + (universe - startUniverse) * (previousDataLength / 3);
        m_ledHardware->setLedColor(led,color(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]));
      }
      previousDataLength = length;

      if (sendFrame)
      {
        m_ledHardware->refresh();
        // Reset universeReceived to 0
        memset(universesReceived, 0, maxUniverses);
      }
    }

    ledGadGetServer& server()    {return m_server;}

protected:
    ledHardWare*            m_ledHardware;
    ledGadget*              m_ledGadget;
    settingsStorage*        m_settingsStorage;
    ledGadGetServer         m_server;
};

#endif // LEDCONTROLLER
