#ifndef LEDWHEEL
#define LEDWHEEL

#include "ledgadget.h"
#include "gyro.h"

class ledWheel :  public ledCircle
{
public:
    ledWheel(extraSettings* s, std::vector<color*> leds, uint16_t first ,uint16_t count , bool reversed = false) :
        ledCircle(s,leds,first ,count ,reversed) , m_gyro(s)
    {
        m_gyro.on();
        m_gyro.firstRead();
    }


    virtual void animateExtra()
    {
        m_gyro.fullRead();

        ledCircle::animateExtra();
    }
protected:
    MPU6050 m_gyro;

};

#endif // LEDWHEEL

