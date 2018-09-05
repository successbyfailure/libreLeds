#ifndef LEDGADGET
#define LEDGADGET

#include "ledController.h"
#include "ledAnimations.h"


class ledGadget
{
public:
  ledGadget(ledController* lc, uint16_t count = 0 , uint16_t startLed = 0 , bool reversed = false)
  {
    _ledController = lc;

    if(count == 0)
        count = lc->ledCount();

    if(!reversed)
        for(uint16_t i = startLed ; i < startLed+count ; i++)
            if((i < lc->ledCount()) && (i >= 0)) _leds.push_back(lc->pixel(i));
    else
        for(uint16_t i = startLed+count ; i >= startLed ; i--)
            if((i<lc->ledCount()) && (i>= 0)) _leds.push_back(lc->pixel(i));

    _brightness = 1.0;
  }

  void update()
  {
    if(_ledController->dmxInUse())
      return;
      if(animate())
      {
        _lastTimeRendered = millis();
        _ledController->showAsync();
      }
  }


  void show()
  {
    if(_ledController->dmxInUse())
      return;
    _ledController->show();
  }


  CRGB getPixelColor(uint16_t index)
  {
    if(index < _leds.size())
      return CRGB();
    CRGB r;
    r.r = _leds[index]->r/_brightness;
    r.g = _leds[index]->g/_brightness;
    r.b = _leds[index]->b/_brightness;
    return r;
  }

  void setColor(CRGB ncolor)
  {
    if(_ledController->dmxInUse())
      return;
    for(uint i = 0 ; i < _leds.size() ;i++)
    {
      _leds[i]->r = ncolor.r * _brightness;
      _leds[i]->g = ncolor.g * _brightness;
      _leds[i]->b = ncolor.b * _brightness;
    }
  }

  void setColor(uint8_t r, uint8_t g, uint8_t b)
  {
      setColor(CRGB(r,g,b));
  }

  void off          ()              {setColor(0,0,0);}
  void white        ()              {setColor(200,200,200);}
  void red          (int r = 200)   {setColor(r,0,0);}
  void green        (int g = 200)   {setColor(0,g,0);}
  void blue         (int b = 200)   {setColor(0,0,b);}
  void randomColor  ()              {setColor(rand()%255,rand()%255,rand()%255);}

  void clearEffects()
  {
    off();
    resetCounters();
    _lastEffect    = "";
    _currentEffect = "";
    _nextEffect    = "";
    show();
  }

  void setEffect(String e)
  {
    clearEffects();
    _currentEffect = e;
  }

  String effect()             {return _currentEffect;}
//efectos

  virtual void fade()         {resetCounters(); _currentEffect = "fade";   }
  virtual void cylon()        {resetCounters(); _currentEffect = "cylon";  }
  virtual void rainbow()      {resetCounters(); _currentEffect = "rainbow";}
  virtual void sparks()       {resetCounters(); _currentEffect = "sparks"; }
  virtual void chaoticLight() {resetCounters(); _currentEffect = "clight"; }


  virtual void glow(CRGB c = CRGB(0,0,0))
  {
    resetCounters();
    _currentEffect = "glow";
    if(c != CRGB(0,0,0))
      _color = c;
  }

  virtual void fadeToColor(CRGB c)
  {
    resetCounters();
    _currentEffect  = "fadeC";
    _lastColor      = _color;
    _color          = c;
  }

  virtual void strobe(CRGB c = CRGB(200,200,200), uint8_t Hz = 10)
  {
      resetCounters();
      _currentEffect = "strobe";
      _counters.c0   = 1000/Hz;
      _color         = c;
      _lastStrobe = millis();
  }

  virtual void instantFade()
  {
    fade();
    while (_currentEffect != "")
    {
      animateFade();
      _ledController->show();
      delay(10);
    }
  }

  virtual void instantFlash(CRGB color = CRGB(100,100,0), uint times = 2,uint length = 250)
  {
    saveState();
    for(uint t = 0 ; t<times ; t++)
    {
      setColor(color);
      show();
      delay(length);
      instantFade();
    }
    restoreState();
  }

protected:
  ledController*     _ledController;
  std::vector<CRGB*> _leds;

  CRGB      _color;
  CRGB      _baseColor;
  CRGB      _lastColor;
  float     _brightness;
  uint32_t  _lastTimeRendered;


  animationCounters _counters;
  animationCounters _scounters;
  uint32_t          _lastStrobe;

  String _lastEffect;
  String _currentEffect;
  String _nextEffect;


  void resetCounters()
  {
    _counters = animationCounters();
  }

  void saveState()
  {
      _scounters  = _counters;
      _lastEffect = _currentEffect;
      _lastColor  = _color;
  }

  void restoreState()
  {
    _counters       = _scounters;
    _currentEffect  = _lastEffect;
    _color          = _lastColor;
  }

  virtual bool animate()
  {
    uint32_t now = millis();
    uint16_t steps = now - _lastTimeRendered;


    if      ( _currentEffect == "fade")   animateFade();
    else if ( _currentEffect == "fadeC")  animateFadeToColor();
    else if ( _currentEffect == "glow")   animateGlow       ( steps);
    else if ( _currentEffect == "cylon")  paintCylon        (_leds,_counters,steps);
    else if ( _currentEffect == "rainbow")paintRainbow      (_leds,_counters,steps);
    else if ( _currentEffect == "sparks") paintSparks       (_leds,steps);
    else if ( _currentEffect == "clight") paintChaoticLight (_leds);
    else if ( _currentEffect == "color")
    {
      setColor(_color);
      return false;
    }
    else if ( _currentEffect == "strobe")
    {
      animateStrobe();
      return false;
    }
    else
      return false;

    return true;
  }

//efectos

  virtual void animateGlow(uint8_t steps = 1)
  {
    float f = (200 - _counters.c0)/150.0;
    if(_counters.c1 == 0)
    {
      _counters.c0+=steps/4;
      if(_counters.c0 >=200.0)
        _counters.c1 = 1;
    }
    else
    {
      _counters.c0-=steps/4;
      if(_counters.c0 <=50)
        _counters.c1 = 0;
    }
    CRGB c;
    if(f >  1.0) f = 1.0;
    if(f <  0.05) f = 0.05;
    c.r = _color.r*f;
    c.g = _color.g*f;
    c.b = _color.b*f;
    setColor(c);
  }

  virtual void animateFade()
  {
    bool allOff = true;
    for(uint16_t i = 0 ; i < _leds.size() ; i++)
    {
      _leds[i]->r *= 0.8;
      _leds[i]->g *= 0.8;
      _leds[i]->b *= 0.8;
      if(*_leds[i] != CRGB(0,0,0))
        allOff = false;
    }

    if(allOff)
    {
      _currentEffect = "";
    }
  }

  virtual void animateFadeToColor()
  {//TODO...
    CRGB c = *_leds[0];

    c.r += (_color.r - c.r)/3;
    c.g += (_color.g - c.g)/3;
    c.b += (_color.b - c.b)/3;
    setColor(c);
    if(*_leds[0] == _color)
      _currentEffect = "color";

  }

  virtual void animateStrobe()
  {
    uint32_t now = millis();
    //Serial.println("Now:" + String(now) + " t: " + String(now - _lastStrobe) + " c: " + String(_leds[0]->r)+ " cs: " + String(_color.r));
    if( (now - _lastStrobe) > _counters.c0)
    {
      setColor(_color);
      _lastStrobe = now - ((now - _lastStrobe - _counters.c0)%_counters.c0);
      show();
    }
    else if( (now - _lastStrobe) > (_counters.c0/2))
    {
      if(*_leds[0] != CRGB(0,0,0))
      {
        off();
        show();
      }
    }
  }

};

class ledBar : public ledGadget
{
public:
ledBar(ledController* lc, uint16_t count = 0 , uint16_t startLed = 0 , bool reversed = false) :
  ledGadget(lc,count,startLed,reversed)
  {

  }
protected:

};




#endif
