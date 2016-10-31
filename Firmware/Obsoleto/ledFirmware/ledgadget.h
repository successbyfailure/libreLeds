#ifndef LEDGADGET
#define LEDGADGET

#include "ledhardware.h"
#include "vector"
#include "msgeq7.h"
#include "qtcompat.h"
#include "protocol.h"

extern uint16_t lastCycleMS;
extern uint16_t thisCycleMS;


struct color{
    color(uint8_t rbyte = 0,uint8_t gbyte = 0,uint8_t bbyte = 0) :
        r(rbyte), g(gbyte) , b(bbyte) {;}

    uint8_t r,g,b;
};

class led{
public:
  led(color* lcolor)
  {
      m_color = lcolor;
  }

  virtual void setColor(color ncolor)
  {
    *m_color = ncolor;
  }

  virtual void setColor(uint8_t r, uint8_t g, uint8_t b)
  {
    *m_color = color(r,g,b);
  }

  void dimm(uint8_t power)
  {
      float p = power /100.0f;
      dimm(p);
  }

  virtual void dimm(float factor = 0.75)
  {
      m_color->r *= factor;
      m_color->g *= factor;
      m_color->b *= factor;
  }

  bool isOn()               {return (m_color->r > 0) || (m_color->b > 0) || (m_color->g > 0);}

  void off()                {setColor(0,0,0);}
  void white()              {setColor(200,200,200);}
  void red  (int r = 200)   {setColor(r,0,0);}
  void green(int g = 200)   {setColor(0,g,0);}
  void blue (int b = 200)   {setColor(0,0,b);}

  void setR(uint8_t val) {m_color->r = val;}
  void setG(uint8_t val) {m_color->g = val;}
  void setB(uint8_t val) {m_color->b = val;}

  color  getColor()          {return *m_color;}
  color* getColorPointer()   {return m_color;}

  uint8_t r()   {return m_color->r;}
  uint8_t g()   {return m_color->g;}
  uint8_t b()   {return m_color->b;}

private:
    color *m_color;
};

static ledGadgetAnimations name2animation(String name)
{
    if     (name == "Fade")
        return animationFade;
    else if(name == "Glow")
        return animationGlow;
    else if(name == "Flash")
        return animationFlash;
    else if(name == "Strobe")
        return animationStrobe;
    else if(name == "Cylon")
        return animationCylon;
    else if(name == "ChaoticLight")
        return animationChaoticLight;
    else if(name == "Rainbow")
        return animationRainbow;
    else if(name == "VUMeter")
        return animationVUMeter;
    else if(name == "VUMeterCentral")
        return animationVUMeterCentral;
    else if(name == "DMX")
        return animationDMX;
    return animationNone;
}

static String animation2name(ledGadgetAnimations a)
{
    if     (a == animationFade)
        return "Fade";
    else if(a == animationGlow)
        return "Glow";
    else if(a == animationFlash)
        return "Flash";
    else if(a == animationStrobe)
        return "Strobe";
    else if(a == animationCylon)
        return "Cylon";
    else if(a == animationChaoticLight)
        return "ChaoticLight";
    else if(a == animationRainbow)
        return "Rainbow";
    else if(a == animationVUMeter)
        return "VUMeter";
    else if(a == animationVUMeterCentral)
        return "VUMeterCentral";
    else if(a == animationDMX)
        return "DMX";
    return "None";
}

static void copyColorArray(std::vector<color*>& src,std::vector<color*>& dest)
{
    int i = 0;
    for( i = 0; ((i < src.size()) && (i < dest.size())) ; i++)
        *dest[i] = *src[i];
    for(int b = i ; b < dest.size() ; b++)
        *dest[i] = color(0,0,0);
}

static std::vector<color*> invertLedOrder(const std::vector<color*>& array)
{
    std::vector<color*> result;
    for(int i = array.size()-1 ; i>= 0  ; i-- )
    {
        result.push_back(array[i]);
    }
    return result;
}

static std::vector<std::vector<color*> > getQuarters(std::vector<color*>& src)
{
    std::vector<std::vector<color*> > result;
    result.push_back(std::vector<color*>());
    uint16_t size = src.size()/4;

    for(int i = 0 ; i < src.size() ; i++)
    {
        if(i > 0 && i%size == 0 )
            result.push_back(std::vector<color*>());
        result.at(result.size()-1).push_back(src.at(i));
    }

    return result;
}

static std::vector<color*> getRightHalf(std::vector<color*>& src)
{
    std::vector<color*> result;
    for(int i = src.size()-1 ; i > src.size()/2 ; i --)
    {
        result.push_back(src.at(i));
    }
    return result;
}

static std::vector<color*> getLeftHalf(std::vector<color*>& src)
{
    std::vector<color*> result;
    for(int i = 0 ; i < src.size()/2 ; i ++)
    {
        result.push_back(src.at(i));
    }
    return result;
}

static void clearColorArray(std::vector<color*>& array)
{
    for(uint16_t i = 0 ; i < array.size() ; i++)
    {
        led l(array.at(i));
        l.off();
    }
}

static color calcVuColor(int level)
{
    if(level <15){
        return color(0,50,160);
    }else if(level <65){
        return color(0,250,0);
    }else if(level <75){
        return color(180,180,0);
    }else{
        return color(250,0,0);
    }
}

static void paintVuMeterBar(std::vector<color*>& array, uint8_t vuPercentage,uint16_t& c0,uint16_t& c1)
{
    if(vuPercentage>100) vuPercentage = 100;
    clearColorArray(array);
    if(vuPercentage > 100) vuPercentage = 100;
    for(int i = 0 ; i < (array.size() * vuPercentage/100.0f) ; i++)
    {
        led l(array.at(i));
        float pos = i/(array.size() * vuPercentage/100.0f);
        int power = pos*vuPercentage;
        l.setColor(calcVuColor(power));
    }
}

static void paintVuMeterCentralBar(std::vector<color*>& array, uint8_t vuPercentage,uint16_t& c0,uint16_t& c1)
{
    if(vuPercentage>100) vuPercentage = 100;
    clearColorArray(array);
    if(vuPercentage > 100) vuPercentage = 100;
    int center   = array.size()/2;
    int sizeUp   = ((array.size() - center) * (vuPercentage/100.0f));
    int sizeDown = (center * (vuPercentage/100.0f));
    for(int i = center ; i < center+sizeUp ; i++)
    {
        led l(array.at(i));
        float pos = i/(center+sizeUp);
        int power = pos*vuPercentage;
        l.setColor(calcVuColor(power));
    }

    for(int i = center ; i > center-sizeDown ; i++)
    {
        led l(array.at(i));
        float pos = (center-sizeDown)/i;
        int power = pos*vuPercentage;
        l.setColor(calcVuColor(power));
    }
}

static void paintCylon(std::vector<color*>& array, uint16_t& c0,uint16_t& c1,uint16_t& c2)
{

    c0 += lastCycleMS;
    if(c0 > 25)
        c0 = 0;
    else
        return;

    uint16_t min = 0;
    uint16_t max = array.size();
    uint16_t count = max - min;

    if(c2 == 0)//incrementando
    {
        int i = 0;
        for(int l = min ; l < max-1 ; l++ )
        {
            led cled(array[l]);
            if(i == c1)
            {
               cled.red();
            }
            else
                cled.dimm();
            if(c1 >= max-1)
            {
                c2 = 1;
                return;
            }
            else
                i++;
        }
        c1++;
    }
    else //decrementando
    {
        uint16_t i = count-1;
        for(int l = max-1 ; l >= min ; l-- )
        {
            led cled(array[l]);
            if(i == c1)
               cled.red();
            else
                cled.dimm();
            if(c1 <= min)
            {
                c1 = 0;
                c2 = 0;
                return;
            }
            else
                i--;
        }
        c1--;
    }
}


class ledGadget{
public:
    ledGadget(extraSettings* s,std::vector<color*> leds, uint16_t first = 0,uint16_t count = 0 , bool reversed = false)
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
        //m_animations.push_back(animationGlow)           ;m_animations.push_back(animationFlash);
        m_animations.push_back(animationFlash);
        m_animations.push_back(animationStrobe)         ;m_animations.push_back(animationCylon);
        m_animations.push_back(animationChaoticLight)   ;m_animations.push_back(animationRainbow);
        m_settings = s;
        setBrigthness(1.0);
    }

    std::vector<ledGadgetAnimations> getAnimations()
    {
        return m_animations;
    }

    virtual void setColor(color ncolor)
    {
      for(int i = 0 ; i < m_leds.size() ;i++)
          setLedColor(i,ncolor);
    }

    virtual void setPixels(std::vector<color*> leds, uint16_t first ,uint16_t count)
    {
        for(int i = first ; i < first+count ; i ++)
            if((first + i) <= m_leds.size() && i < leds.size())
                m_leds.push_back(leds[i]);
    }

    virtual void setColor(uint8_t r, uint8_t g, uint8_t b)
    {
      setColor(color(r,g,b));
    }

    virtual void setLedColor(uint16_t index, color ncolor)
    {
        led l(m_leds[index]);
        l.setColor(ncolor);
        l.dimm(m_brightness);
    }

    void dimm(uint8_t power)
    {
        float p = power /100.0f;
        dimm(p);
    }

    void setBrigthness(float factor = 1.0f) {m_brightness = factor;}

    virtual void dimm(float factor = 0.75)
    {
        for(int i = 0 ; i < m_leds.size() ;i++)
            led(m_leds.at(i)).dimm(factor);
    }

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

    virtual void setAnimation(ledGadgetProtocolPacket& p)
    {
        setAnimation((ledGadgetAnimations)p.intVal0);
    }

    virtual void setAnimation(ledGadgetAnimations a)
    {
        resetAnimation();
        m_currentAnimation = a;
        Serial.print("setAnimation: ");Serial.println(a);
    }

    ledGadgetAnimations currentAnimation() {return m_currentAnimation;}

    virtual void resetAnimation()
    {
        m_lastAnimation    = m_currentAnimation;
        m_currentAnimation = animationNone;
        m_nextAnimation    = animationNone;
        resetCounters();
    }

    void animate()
    {
        if(m_currentAnimation == animationNone)
            return;
        else if(m_currentAnimation ==animationFade)
            animateFade();
        else if(m_currentAnimation == animationGlow)
            animateGlow();
        else if(m_currentAnimation == animationFlash)
            animateFlash();
        else if(m_currentAnimation == animationStrobe)
            animateStrobe();
        else if(m_currentAnimation == animationCylon)
            animateCylon();
        else if(m_currentAnimation == animationChaoticLight)
            animateChaoticLight();
        else if(m_currentAnimation == animationRainbow)
            animateRainbow();
        else
            animateExtra();
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

    virtual void animateExtra() {;}
    void resetCounters()        {m_c0 = 0,m_c1 = 0,m_c2 = 0,m_c3 = 0,m_c4 = 0,m_c5 = 0,m_c6 = 0;}

    extraSettings*                    m_settings;
    std::vector<color*>               m_leds;
    std::vector<ledGadgetAnimations>  m_animations;
    ledGadgetAnimations               m_lastAnimation       = animationNone;
    ledGadgetAnimations               m_currentAnimation    = animationNone;
    ledGadgetAnimations               m_nextAnimation       = animationNone;

    float         m_brightness;
    uint16_t      m_c0 = 0,m_c1 = 0,m_c2 = 0,m_c3 = 0,m_c4 = 0,m_c5 = 0,m_c6 = 0; // Contadores para los efectos


    virtual void animateFade()
    {
        bool all_off = true;
        for(int i = 0 ; i < m_leds.size() ; i++)
        {
            led l(m_leds[i]);
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
        uint16_t val = 255 - m_c0;
        setColor(color(val,val,val));
        m_c0 += lastCycleMS;
        if(m_c0 > 255)
        {
            off();
            resetCounters();
            m_currentAnimation = m_nextAnimation;
        }
    }

    virtual void animateStrobe()
    {
        m_c0 += lastCycleMS;
        if(m_c2)
        {
            m_c2 = 0;
            off();
            return;
        }

        if( (m_c1 + thisCycleMS) >= m_c0)
        {
            white();
            m_c2 = 1;
            m_c1 = thisCycleMS + m_c1 - m_c0;
        }
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
            led l(m_leds[i]);
            if(random == 0)
                setLedColor(i,color(250,0,0));
            else if(random == 1)
                setLedColor(i,color(0,250,0));
            else if(random == 2)
                setLedColor(i,color(0,0,250));
        }
    }

    virtual void animateRainbow()
    {
        if(m_c1 > 256*5) m_c1 = 0;

        for(uint16_t i = 0;  i < m_leds.size() ; i++)
        {
            uint16_t WheelPos = 255 - (((i * 256 / m_leds.size()) + m_c1) & 255);
            led l(m_leds[i]);
            if(WheelPos < 85)
            {
                l.setColor(255 - WheelPos * 3, 0, WheelPos * 3);
            }
            else if(WheelPos < 170)
            {
                WheelPos -= 85;
                l.setColor(0, WheelPos * 3, 255 - WheelPos * 3);
            }
            else
            {
                WheelPos -= 170;
                l.setColor(WheelPos * 3, 255 - WheelPos * 3, 0);
            }
        }

        if(lastCycleMS>4)
            m_c1 += (lastCycleMS)/4;
        else
            m_c1 += 1;
    }
};

class spotLed : public ledGadget
{
public:
    spotLed(extraSettings* s,std::vector<color*> leds, uint16_t first = 0 ,uint16_t count = 1):
        ledGadget(s,leds,first ,count ,false)
    {

    }
};

class ledBar : public ledGadget
{
public:
    ledBar(extraSettings* s,std::vector<color*> leds, uint16_t first = 0,uint16_t count = 0 , bool reversed = false) :
        ledGadget(s,leds,first ,count ,reversed) , m_eq(s)
    {
        m_animations.push_back(animationVUMeterCentral);
        m_animations.push_back(animationVUMeter);
        m_eq.setup();
    }

protected:
    MSGEQ7 m_eq;
    virtual void animateExtra()
    {
        if(m_currentAnimation == animationVUMeter)
            animateVUMeter();
        else if(m_currentAnimation == animationVUMeterCentral)
            animateVUMeterCentral();
    }


    virtual void animateVUMeter()
    {
        paintVuMeterBar(m_leds,m_eq.readBand(1)*100/650,m_c4,m_c5);
    }

    virtual void animateVUMeterCentral()
    {
        paintVuMeterCentralBar(m_leds,m_eq.readBand(1)*100/650,m_c4,m_c5);
    }
};

class ledCircle : public ledBar
{
public:
    ledCircle(extraSettings* s,std::vector<color*> leds,uint16_t first = 0,uint16_t count = 0 , bool reversed = false) :
        ledBar(s,leds,first ,count ,reversed)
    {
        m_barSides.push_back(getRightHalf(leds));
        m_barSides.push_back(getLeftHalf(leds));
        m_barQuarters = getQuarters(leds);
    }


protected:

    std::vector<std::vector<color*> > m_barSides;
    std::vector<std::vector<color*> > m_barQuarters;

    virtual void animateExtra()
    {
        ledBar::animateExtra();
    }


    virtual void animateVUMeter()
    {
        paintVuMeterBar(m_barQuarters[0],m_eq.readBand(1)*100/650,m_c4,m_c5);
        for(int b = 1 ; b < m_barQuarters.size() ; b++)
            copyColorArray(m_barQuarters[0],m_barQuarters[b]);
    }

    virtual void animateVUMeterCentral()
    {
        paintVuMeterCentralBar(m_barQuarters[0],m_eq.readBand(1)*100/650,m_c4,m_c5);
        for(int b = 1 ; b < m_barQuarters.size() ; b++)
            copyColorArray(m_barQuarters[0],m_barQuarters[b]);
    }

    virtual void animateCylon()
    {
        paintCylon(m_barQuarters[0],m_c0,m_c1,m_c2);
        std::vector<color*> inverted = invertLedOrder(m_barQuarters[0]);
        copyColorArray(inverted,m_barQuarters[1]);
        copyColorArray(m_barQuarters[0],m_barQuarters[2]);
        copyColorArray(m_barQuarters[1],m_barQuarters[3]);
    }
};


class ledMatrix : public ledGadget
{

};

#endif // LEDGADGET
