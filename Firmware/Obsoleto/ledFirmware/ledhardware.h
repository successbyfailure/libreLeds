#ifndef LEDHARDWARE
#define LEDHARDWARE

#include "ledgadget.h"

class ledHardWare{
public:
    ledHardWare()                              {;}

    virtual void setup()                     = 0;
    virtual void refresh()                   = 0;
    virtual void setBrightness(float b)        {;}

    virtual led getLed(uint16_t x,uint16_t y)   {if(!isMatrix()) return led(&m_colorArray[0]);}
    led getLed(uint16_t index)                  {return led(&m_colorArray[index]);}
    std::vector<color>* colorArray()            {return &m_colorArray;}
    uint16_t ledCount()                         {return m_colorArray.size();}
    bool isMatrix()                             {return m_isMatrix;}

    void setLedColor(uint16_t index, color ncolor)
    {
        if(index > m_colorArray.size())
            return;
        m_colorArray[index] = ncolor;
    }

    void test()
    {
      for( uint16_t i = 0 ; i < ledCount() ; i++)
      {
        getLed(i).white();
        if(i>=5)
            getLed(i-5).off();
        if(i%2 == 0)refresh();
        if(i%100 == 0)yield();
      }
    }

protected:
    std::vector<color> m_colorArray;
    bool                m_isMatrix = false;
    void initColorArray(uint16_t ledCount)
    {
        for(int i = 0 ; i < ledCount ; i++)
            m_colorArray.push_back(color());
    }
};

class ws2812Strip : public ledHardWare{
public:
    ws2812Strip(int8_t pin = -1, uint16_t lednr = 25, float bright = 1.0) : ledHardWare(),
        m_strip(lednr, pin, NEO_GRB + NEO_KHZ800) , m_maxBright(bright)
    {
        initColorArray(lednr);
    }

    void setup()
    {
        m_strip.begin();
    }

    void refresh()
    {
        for(uint16_t i=0; i<ledCount() ;i++)
        {
            m_strip.setPixelColor(i, (m_colorArray[i].r * m_maxBright),
                                     (m_colorArray[i].g * m_maxBright),
                                     (m_colorArray[i].b * m_maxBright));
        }
        m_strip.show();
    }

    virtual void setBrightness(float b)
    {
        m_maxBright = b;
    }

protected:
    float               m_maxBright;
    Adafruit_NeoPixel   m_strip;
};

class ws2812Matrix : public ledHardWare{

};

class apa102Strip : public ledHardWare{

};

#endif // LEDHARDWARE

