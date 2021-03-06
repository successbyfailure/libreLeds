#ifndef LEDWHEEL
#define LEDWHEEL

#include "ledgadget.h"
#include "gyro.h"

class ledWheel :  public ledCircle
{
public:
    ledWheel(extraSettings* s,std::vector<CRGB*> leds, uint16_t first ,uint16_t count , bool reversed = false) :
        ledCircle(s,leds,first ,count ,reversed) , m_gyro(s)
    {
        m_gyro.init();
        m_animations.push_back(animationSpeed2Light);
    }

    virtual void readExtraSensors()
    {
        m_gyro.fullRead();
    }

    virtual void animateExtra()
    {
        ledCircle::animateExtra();
        if(m_currentAnimation == animationSpeed2Light)
            animateSpeed2Light();
    }

protected:
    MPU6050 m_gyro;

    void animateSpeed2Light()
    {
        uint16_t val = m_gyro.GyX;
        uint8_t r,g,b;
        r=0,g=0,b=0;

        if(val > -220)//compensar que el valor estatico del gx es ~ -250
        {
            b = abs(val)*255/32768.0f;
            if(b > 100)
            {
                g = b * 0.5;
            }
        }
        if(val < -260)
        {
            r = abs(val)*255/32768.0f;
            if(r > 100)
            {
                g = r * 0.5;
            }
        }
        setColor(r,g,b);
    }
};

#endif // LEDWHEEL


