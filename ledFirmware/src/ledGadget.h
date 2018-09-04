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

  void off()                {setColor(0,0,0);}
  void white()              {setColor(200,200,200);}
  void red  (int r = 200)   {setColor(r,0,0);}
  void green(int g = 200)   {setColor(0,g,0);}
  void blue (int b = 200)   {setColor(0,0,b);}



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



protected:
  ledController*     _ledController;
  std::vector<CRGB*> _leds;

  CRGB      _baseColor;
  CRGB      _lastColor;
  float     _brightness;
  uint32_t  _lastTimeRendered;

  animationCounters _counters;

  String _lastEffect;
  String _currentEffect;
  String _nextEffect;


  void resetCounters()
  {
    _counters = animationCounters();
  }

  virtual bool animate()
  {
    uint32_t now = millis();
    uint16_t steps = now - _lastTimeRendered;
    if(steps < 5)
      return false;

    if( _currentEffect == "cylon")
    {
      paintCylon(_leds,_counters,steps);
    }
    else if( _currentEffect == "rainbow")
    {
      paintRainbow(_leds,_counters,steps);
    }
    else if( _currentEffect == "sparks")
    {
      paintSparks(_leds);
    }
    else
    {
      return false;
    }
    return true;
  }

//efectos



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
