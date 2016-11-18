#ifndef LEDGADGET
#define LEDGADGET

#include "animations.h"
#include "ledhardware.h"
#include "vector"
#include "msgeq7.h"
#include "qtcompat.h"
#include "protocol.h"

class ledGadget{
public:
    ledGadget(extraSettings* s,std::vector<CRGB*>& leds, uint16_t first = 0,uint16_t count = 0 , bool reversed = false)
    {
        if(count == 0)
            count = leds.size();
        if(!reversed)
            for(uint16_t i = first ; i < first+count ; i++)
                if((i < leds.size()) && (i >= 0)) m_leds.push_back(leds.at(i));
        else
            for(uint16_t i = first+count ; i >= first ; i--)
                if((i<leds.size()) && (i>= 0)) m_leds.push_back(leds.at(i));

        m_animations.push_back(animationNone)           ;m_animations.push_back(animationFade);
        m_animations.push_back(animationFlash)          ;m_animations.push_back(animationStrobe);
        m_animations.push_back(animationSparks);
        m_animations.push_back(animationCylon);
        m_animations.push_back(animationChaoticLight)   ;m_animations.push_back(animationRainbow);
        m_settings = s;
        m_peakCallBack = 0;
    }

    std::vector<ledGadgetAnimations> getAnimations()
    {
        return m_animations;
    }

    virtual void setColor(CRGB ncolor)
    {
        resetAnimation();
        for(int i = 0 ; i < m_leds.size() ;i++)
      {
        *m_leds[i] = ncolor;
      }
    }

    virtual void setColor(uint8_t r, uint8_t g, uint8_t b)
    {
        setColor(CRGB(r,g,b));
    }

    virtual void dimm(uint8_t power)
    {
        for(int i = 0 ; i < m_leds.size() ;i++)
        {
          ledHardWare::led led(m_leds[i]);
          led.dimm(power);
        }
    }

    virtual void dimm(float factor = 0.75)
    {
        dimm(uint8_t(255*factor));
    }

    void setBrigthness(float factor = 1.0f) {}

    void off()                {setColor(0,0,0);resetAnimation();}
    void white()              {setColor(200,200,200);}
    void red  (int r = 200)   {setColor(r,0,0);}
    void green(int g = 200)   {setColor(0,g,0);}
    void blue (int b = 200)   {setColor(0,0,b);}

    virtual void setDmxMode()
    {
        resetAnimation();
        m_currentAnimation = animationDMX;
    }

    void setPeakCallback(void (*fptr)(float bpm))
    {
      m_peakCallBack = fptr;
    }

    virtual MSGEQ7* eq() {return 0;}

    virtual void setAnimation(ledGadgetProtocolPacket& p)
    {
        setAnimation((ledGadgetAnimations)p.intVal0);
    }

    virtual void setAnimation(ledGadgetAnimations a)
    {
        resetAnimation();
        m_currentAnimation = a;
    }

    virtual void setNextAnimation(ledGadgetAnimations a)
    {
        m_nextAnimation = a;
    }

    ledGadgetAnimations currentAnimation() {return m_currentAnimation;}

    virtual void resetAnimation()
    {
        m_lastAnimation    = m_currentAnimation;
        m_currentAnimation = animationNone;
        m_nextAnimation    = animationNone;
        resetCounters();
    }

    virtual void readSensors() {;}

    void animate()
    {
        if(m_currentAnimation == animationNone)
        {
            timeSinceLastFrameMS = 0;
            return;
        }

        else if(m_currentAnimation ==animationFade)
            animateFade();
        else if(m_currentAnimation == animationGlow)
            animateGlow();
        else if(m_currentAnimation == animationFlash)
            animateFlash();
        else if(m_currentAnimation == animationStrobe)
            animateStrobe();
        else if(m_currentAnimation == animationSparks)
            animateSparks();
        else if(m_currentAnimation == animationCylon)
            animateCylon();
        else if(m_currentAnimation == animationChaoticLight)
            animateChaoticLight();
        else if(m_currentAnimation == animationRainbow)
            animateRainbow();
        else
            animateExtra();

        timeSinceLastFrameMS = 0;
    }

    virtual void fade()
    {
        resetCounters();
        m_lastAnimation     = m_currentAnimation;
        m_currentAnimation  = animationFade;
        m_nextAnimation     = animationNone;
    }

    virtual void fadeToNext()
    {
        resetCounters();
        m_lastAnimation     = m_currentAnimation;
        m_currentAnimation  = animationFade;
    }

    virtual void Glow()
    {
        resetCounters();
        m_lastAnimation     = m_currentAnimation;
        m_currentAnimation  = animationGlow;
    }

    virtual void flash()
    {
        resetCounters();
        m_nextAnimation     = m_currentAnimation;
        m_currentAnimation  = animationFlash;
    }

    virtual void strobe(uint8_t Hz)
    {
        resetCounters();
        m_lastAnimation     = m_currentAnimation;
        m_currentAnimation  = animationStrobe;
        m_c0 = 1000/Hz;
    }

    virtual void cylon()
    {
        resetCounters();
        m_lastAnimation     = m_currentAnimation;
        m_currentAnimation  = animationCylon;
    }

    virtual void chaoticLigth()
    {
        resetCounters();
        m_lastAnimation     = m_currentAnimation;
        m_currentAnimation  = animationChaoticLight;
    }

    virtual void rainbow()
    {
        resetCounters();
        m_lastAnimation     = m_currentAnimation;
        m_currentAnimation  = animationRainbow;
    }

protected:

    virtual void animateExtra()     {;}
    void         resetCounters()    {m_c0 = 0,m_c1 = 0,m_c2 = 0,m_c3 = 0,m_c4 = 0,m_c5 = 0,m_c6 = 0;}

    extraSettings*                    m_settings;
    std::vector<CRGB*>                m_leds;
    std::vector<ledGadgetAnimations>  m_animations;
    ledGadgetAnimations               m_lastAnimation       = animationNone;
    ledGadgetAnimations               m_currentAnimation    = animationNone;
    ledGadgetAnimations               m_nextAnimation       = animationNone;

    float         m_brightness;
    uint16_t      m_c0 = 0,m_c1 = 0,m_c2 = 0,m_c3 = 0,m_c4 = 0,m_c5 = 0,m_c6 = 0; // Contadores para los efectos

    void (*m_peakCallBack)(float bpm);

    virtual void animateFade()
    {
        bool all_off = true;
        for(uint16_t i = 0 ; i < m_leds.size() ; i++)
        {
            ledHardWare::led l(m_leds[i]);
            if(l.isOn())
            {
                l.dimm();
                all_off=false;
            }
        }

        if(all_off)
        {
            resetCounters();
            m_currentAnimation  = m_nextAnimation;
            m_nextAnimation     = animationNone;
        }
    }

    virtual void animateGlow()
    {

    }

    virtual void animateFlash()
    {
        uint16_t val = 254 - m_c0;
        setColor(CRGB(val,val,val));
        m_c0 += timeSinceLastFrameMS;
        if(m_c0 > 254)
        {
            off();
            resetCounters();
            m_currentAnimation = m_nextAnimation;
        }
    }

    virtual void animateStrobe()
    {
        if(m_c0 == 0) m_c0 = 1000 / 5;
        m_c1 += timeSinceLastFrameMS;
        if(m_c2)
        {
           if(m_c1+ thisCycleMS > m_c0/3)
           {
               m_c2 = 0;
               setColor(CRGB(0,0,0));
           }
               return;
        }

        if( (m_c1 + thisCycleMS) >= m_c0)
        {
            white();
            m_c2 = 1;
            m_c1 = 0;
        }
    }

    virtual void animateSparks()
    {
        paintSparks(m_leds);
    }

    virtual void animateCylon()
    {
        paintCylon(m_leds,m_c0,m_c1,m_c2);
    }

    virtual void animateChaoticLight()
    {
        for(uint16_t i = 0 ; i < m_leds.size() ; i++)
        {
            uint8_t random = rand() % 3;
            ledHardWare::led led(m_leds[i]);
            if(random == 0)
                led.setColor(CRGB(250,0,0));
            else if(random == 1)
                led.setColor(CRGB(0,250,0));
            else if(random == 2)
                led.setColor(CRGB(0,0,250));
        }
    }

    virtual void animateRainbow()
    {
        paintRainbow(m_leds,m_c1);
    }
};

class spotLed : public ledGadget
{
public:
    spotLed(extraSettings* s,std::vector<CRGB*> leds, uint16_t first = 0 ,uint16_t count = 1):
        ledGadget(s,leds,first ,count ,false)
    {

    }
};

class ledBar : public ledGadget
{
public:
    ledBar(extraSettings* s,std::vector<CRGB*> leds, uint16_t first = 0,uint16_t count = 0 , bool reversed = false) :
        ledGadget(s,leds,first ,count ,reversed) , m_eq(s)
    {
        m_animations.push_back(animationVUMeterCentral);
        m_animations.push_back(animationVUMeter);
        m_animations.push_back(animationFlashOnPeak);
        m_eq.setup();
        m_peakHappend = false;
    }

    virtual void readSensors()
    {

    }

    virtual MSGEQ7* eq() {return &m_eq;}

protected:
    MSGEQ7 m_eq;
    bool   m_peakHappend;

    virtual void animateExtra()
    {
        m_eq.analize();
        m_peakHappend = m_eq.peakHappened();
        if(m_currentAnimation == animationVUMeter)
            animateVUMeter();
        else if(m_currentAnimation == animationVUMeterCentral)
            animateVUMeterCentral();
        else if(m_currentAnimation == animationFlashOnPeak)
            animateFlashOnPeak();

        if(m_peakHappend)
            if(m_peakCallBack) (*m_peakCallBack)(m_eq.bpms());
    }

    virtual void animateFlashOnPeak()
    {
        if(m_peakHappend)
        {
            m_c0 = 0;
        }
        if(m_c0 >254)
            return;
        uint16_t val = 255-m_c0;
        setColor(CRGB(val,val,val));
        m_c0 += timeSinceLastFrameMS;
        if(m_c0 > 254)
        {
            setColor(CRGB(0,0,0));
        }
    }

    virtual void animateVUMeter()
    {
        paintVuMeterBar(m_leds,m_eq.vuLevel());
    }

    virtual void animateVUMeterCentral()
    {
        paintVuMeterCentralBar(m_leds,m_eq.vuLevel());
    }
};

class ledCircle : public ledBar
{
public:
    ledCircle(extraSettings* s,std::vector<CRGB*> leds,uint16_t first = 0,uint16_t count = 0 , bool reversed = false) :
        ledBar(s,leds,first ,count ,reversed)
    {
        m_barSides.push_back(getRightHalf(leds));
        m_barSides.push_back(getLeftHalf(leds));
        m_barQuarters = getQuarters(leds);
    }


protected:

    std::vector<std::vector<CRGB*> > m_barSides;
    std::vector<std::vector<CRGB*> > m_barQuarters;

    virtual void animateExtra()
    {
        ledBar::animateExtra();
    }


    virtual void animateVUMeter()
    {
        uint16_t val = m_eq.vuLevel();
        for(uint16_t i = 0 ; i < m_barQuarters.size() ; i++)
        {
            paintVuMeterBar(m_barQuarters[i],val);
        }
    }

    virtual void animateVUMeterCentral()
    {
        uint16_t val = m_eq.vuLevel();
        for(uint16_t i = 0 ; i < m_barQuarters.size() ; i++)
        {
            paintVuMeterCentralBar(m_barQuarters[i],val);
        }
    }

    virtual void animateCylon()
    {
        paintCylon(m_barQuarters[0],m_c0,m_c1,m_c2);
        for(uint16_t i = 1 ; i < m_barQuarters.size() ; i++)
        {
            copyLeds(m_barQuarters[0],m_barQuarters[i]);
        }
    }
};


class ledMatrix : public ledBar
{
public:
    ledMatrix(extraSettings* s,ledMatrixHW* matrix,uint16_t first = 0,uint16_t count = 0 , bool reversed = false) :
            ledBar(s,matrix->getColorArray(),first ,count ,reversed)
    {
        m_animations.push_back(animationVUMeterHist);
        m_animations.push_back(animationEQ);
        m_animations.push_back(animationEQCenter);
        m_animations.push_back(animationEQHist);
        m_animations.push_back(animationScrollText);
        m_animations.push_back(animationShowBPM);
        m_sizeX = s->ledMatrixXSize * s->ledMatrixXTiles;
        m_sizeY = s->ledMatrixYSize * s->ledMatrixYTiles;
        Serial.print("Setting up matrix: w:");Serial.print(m_sizeX);
        Serial.print(" h:");Serial.println(m_sizeY);
        m_matrix = matrix;
        m_nextAnimation = animationVUMeterHist;
    }

protected:
    ledMatrixHW*    m_matrix;
    uint16_t        m_sizeX;
    uint16_t        m_sizeY;


    virtual void animateExtra()
    {
        ledBar::animateExtra();

        if     (m_currentAnimation == animationEQ)
            animateEQ();
        else if(m_currentAnimation == animationEQCenter)
            animateEQCenter();
        else if(m_currentAnimation == animationEQHist)
            animateEQHist();
        else if(m_currentAnimation == animationVUMeterHist)
            animateVUMeterHist();
        else if(m_currentAnimation == animationScrollText)
            animateScrollText();
        else if(m_currentAnimation == animationShowBPM)
            animateShowBPM();
    }

    virtual void animateVUMeter()
    {
        uint16_t val = m_eq.vuLevel();
        for(uint16_t i = 0 ; i < m_matrix->height() ; i++)
        {
            std::vector<CRGB*> row = m_matrix->getRow(i);
            paintVuMeterCentralBar(row,val);
        }
    }

    virtual void animateVUMeterCentral()
    {
        clearLedArray(m_leds);
        uint16_t val = m_eq.vuLevel();
        for(uint16_t i = 0 ; i < m_matrix->width() ; i++)
        {
            std::vector<CRGB*> col = m_matrix->getCol(i);
            paintVuMeterCentralBar(col,val);
        }
    }

    void animateVUMeterHist()
    {
        clearLedArray(m_leds);
        int i;
        for(i = 0 ; ( (i < m_matrix->width()) && (i < m_eq.vuHistory().size()) ) ; i++)
        {
            std::vector<CRGB*> col = m_matrix->getCol(i);
            paintVuMeterCentralBar(col,m_eq.vuHistory()[m_eq.vuHistory().size()-1-i]);
        }
    }

    virtual void animateEQ()
    {
        paintEQ(m_leds,m_matrix->width(),m_matrix->height(),m_eq.spectrum());
    }

    virtual void animateEQCenter()
    {
        paintEQCenter(m_leds,m_matrix->width(),m_matrix->height(),m_eq.spectrum());
    }

    virtual void animateEQHist()
    {
        std::vector<std::vector<uint8_t> > spectrumHist = m_eq.eqHistory();
        paintEQHist(m_leds,m_matrix->width(),m_matrix->height(),spectrumHist);
    }

    virtual void animateCylon()
    { 
        uint16_t c0 = m_c0,c1 = m_c1,c2 = m_c2;
        for(uint16_t i = 0 ; i < m_sizeY ; i++)
        {
            std::vector<CRGB*> row = m_matrix->getRow(i);
            m_c0 = c0,m_c1 = c1,m_c2 = c2;
            paintCylon(row,m_c0,m_c1,m_c2);
        }
    }

    virtual void animateScrollText()
    {
        if(m_c0 > 120)
            m_c0 = 0;
        else
            m_c0++;

        m_matrix->drawText("LibreLeds V0.1",(60-m_c0),0);
    }

    virtual void animateShowBPM()
    {
        String str;
        str = m_eq.bpms();
        m_matrix->drawText(str,0,0,calcVuColor(m_eq.vuLevel()));

        ledHardWare::led led(m_leds[0]);
        if(m_peakHappend)
            led.blue(255);
        else
            led.dimm();

        if(m_matrix->height() > 7)
            for(int i = 7 ; i <m_matrix->height() ; i++)
            {
                std::vector<CRGB*> row = m_matrix->getRow(i);
                paintVuMeterCentralBar(row,m_eq.vuLevel());
            }
    }
};

#endif // LEDGADGET
