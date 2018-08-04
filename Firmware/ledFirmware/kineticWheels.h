#ifndef KWHEELS
#define KWHEELS

#include "ledgadget.h"



class kineticWheels :  public ledCircle
{
public:
    kineticWheels(extraSettings* s,std::vector<CRGB*> leds, uint16_t first ,uint16_t count , bool reversed = false) :
        ledCircle(s,leds,first ,count ,reversed)
    {
		pinMode(D1,OUTPUT);
		pinMode(D2,OUTPUT);
		pinMode(D5,OUTPUT);
		pinMode(D6,OUTPUT);
		analogWrite(D1,350);
		analogWrite(D6,1000);
    }


    virtual void animateExtra()
    {
        ledCircle::animateExtra();

    }

protected:
	
};

#endif
