#ifndef LIBRESABER_H
#define LIBRESABER_H

#include "ledgadget.h"
#include "gyro.h"

class libreSaber :  public ledBar
{
public:
    libreSaber(extraSettings* s,std::vector<CRGB*> leds, uint16_t first ,uint16_t count , bool reversed = false) :
        ledBar(s,leds,first ,count ,reversed) , m_gyro(s)
    {
        m_gyro.init();
        m_animations.push_back(animationSpeed2Light);
        m_animations.push_back(animationSaberOn);
        m_animations.push_back(animationSaber);
        m_animations.push_back(animationSaberOff);
    }

    virtual void readExtraSensors()
    {
        m_gyro.fullRead();
        m_gyro.outputSerialData();
    }

    virtual void animateExtra()
    {
        ledBar::animateExtra();
        if(m_currentAnimation == animationSpeed2Light)
            animateSpeed2Light();
        else if(m_currentAnimation == animationSaberOn)
            animateSaberOn();
        else if(m_currentAnimation == animationSaber)
            animateSaber();
        else if(m_currentAnimation == animationSaberOff)
            animateSaberOff();
    }

    void saberOn()
    {
        resetAnimation();
        m_currentAnimation = animationSaberOn;
        m_nextAnimation    = animationSaber;
    }

    void saberOff()
    {
        m_currentAnimation = animationSaberOff;
        m_nextAnimation    = animationNone;
    }

protected:
    MPU6050 m_gyro;


    void onButton0Press()
    {
        saberOn();
    }

    void onButton1Press()
    {
        saberOff();
    }

    void onButton0LongPress()
    {;}

    void onButton1LongPress()
    {;}


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

    void animateSaberOn()
    {

    }

    void animateSaber()
    {

    }

    void animateSaberOff()
    {

    }

};

#endif // LIBRESABER_H
