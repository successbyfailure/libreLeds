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
        m_gyro.on();
        m_gyro.firstRead();
    }

    virtual void readSensors()
    {
        m_gyro.fullRead();
    }

    virtual void animateExtra()
    {
        ledCircle::animateExtra();
    }
protected:
    MPU6050 m_gyro;

};

#endif // LEDWHEEL

