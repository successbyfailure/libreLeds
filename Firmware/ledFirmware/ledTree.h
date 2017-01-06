#ifndef TREELEDGADGET_H
#define TREELEDGADGET_H

#include "ledgadget.h"

class ledTree : public ledBar
{
public:
    ledTree(extraSettings* s,std::vector<CRGB*> leds,uint16_t first = 0,uint16_t count = 0 , bool reversed = false) :
        ledBar(s,leds,first ,count ,reversed)
    {
    //V0
        m_VertBars.push_back(std::vector<CRGB*>());
        for(int i = 0 ; i < 23 ; i ++)
        {
            m_VertBars[0].push_back(leds[i]);
        }
    //V1
        m_VertBars.push_back(std::vector<CRGB*>());
        for(int i = 23 ; i < 45 ; i ++)
        {
            m_VertBars[1].push_back(leds[i]);
        }
        m_VertBars[1] = invertLedOrder(m_VertBars[1]);
    //V2
        m_VertBars.push_back(std::vector<CRGB*>());
        for(int i = 45 ; i < 60 ; i ++)
        {
            m_VertBars[2].push_back(leds[i]);
        }
    //V3
        m_VertBars.push_back(std::vector<CRGB*>());
        for(int i = 60 ; i < 74 ; i ++)
        {
            m_VertBars[3].push_back(leds[i]);
        }
        m_VertBars[3] = invertLedOrder(m_VertBars[3]);

    //H0
        m_Horizbars.push_back(std::vector<CRGB*>());
        for(int i = 74 ; i < 79 ; i ++)
        {
            m_Horizbars[0].push_back(leds[i]);
        }
    //H1
        m_Horizbars.push_back(std::vector<CRGB*>());
        for(int i = 79 ; i < 84 ; i ++)
        {
            m_Horizbars[1].push_back(leds[i]);
        }
        m_Horizbars[1] = invertLedOrder(m_Horizbars[1]);
    //H2
        m_Horizbars.push_back(std::vector<CRGB*>());
        for(int i = 84 ; i < 89 ; i ++)
        {
            m_Horizbars[2].push_back(leds[i]);
        }
    //H3
        m_Horizbars.push_back(std::vector<CRGB*>());
        for(int i = 89 ; i < 94 ; i ++)
        {
            m_Horizbars[3].push_back(leds[i]);
        }
        m_Horizbars[3] = invertLedOrder(m_Horizbars[3]);
    //H4
        m_Horizbars.push_back(std::vector<CRGB*>());
        for(int i = 94 ; i < 99 ; i ++)
        {
            m_Horizbars[4].push_back(leds[i]);
        }
    //H5
        m_Horizbars.push_back(std::vector<CRGB*>());
        for(int i = 99 ; i < leds.size() ; i ++)
        {
            m_Horizbars[5].push_back(leds[i]);
        }
        m_Horizbars[5] = invertLedOrder(m_Horizbars[5]);
    }


protected:

    std::vector<std::vector<CRGB*> > m_VertBars;
    std::vector<std::vector<CRGB*> > m_Horizbars;

    virtual void animateExtra()
    {
        ledBar::animateExtra();
    }



    virtual void animateVUMeter()
    {
        uint16_t val = m_eq.vuLevel();
        for(uint16_t i = 0 ; i < m_VertBars.size() ; i++)
        {
            paintVuMeterBar(m_VertBars[i],val);
        }

        for(uint16_t i = 0 ; i < m_Horizbars.size() ; i++)
        {
            paintVuMeterBar(m_Horizbars[i],val);
        }

    }

    virtual void animateVUMeterCentral()
    {
        uint16_t val = m_eq.vuLevel();
        for(uint16_t i = 0 ; i < m_VertBars.size() ; i++)
        {
            paintVuMeterCentralBar(m_VertBars[i],val);
        }

        for(uint16_t i = 0 ; i < m_Horizbars.size() ; i++)
        {
            paintVuMeterCentralBar(m_Horizbars[i],val);
        }
    }

    virtual void animateCylon()
    {
        paintCylon(m_VertBars[0],m_c0,m_c1,m_c2);
        for(uint16_t i = 1 ; i < m_VertBars.size() ; i++)
        {
            copyLeds(m_VertBars[0],m_VertBars[i]);
        }

        for(uint16_t i = 0 ; i < m_Horizbars.size() ; i++)
        {
            copyLeds(m_VertBars[0],m_Horizbars[i]);
        }
    }

    virtual void animateRainbow()
    {
        paintRainbow(m_VertBars[0],m_c1);
        for(uint16_t i = 1 ; i < m_VertBars.size() ; i++)
        {
            copyLeds(m_VertBars[0],m_VertBars[i]);
        }
        for(uint16_t i = 0 ; i < m_Horizbars.size() ; i++)
        {
            copyLeds(m_VertBars[0],m_Horizbars[i]);
        }
    }

};

#endif // TREELEDGADGET_H

