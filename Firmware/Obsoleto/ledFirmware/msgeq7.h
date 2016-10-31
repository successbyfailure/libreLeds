#ifndef MSGEQ7CLASS
#define MSGEQ7CLASS

#include "settings.h"

class MSGEQ7
{
public:

    MSGEQ7(extraSettings* s) :
        m_resetPin(s->eqReset),m_linePin(s->eqOut),m_strobePin(s->eqStrobe) {}

    MSGEQ7(uint8_t linePin , uint8_t resetPin, uint8_t strobePin) :
        m_resetPin(resetPin),m_linePin(linePin),m_strobePin(strobePin)      {}

    void setup()
    {
        pinMode(m_resetPin  , OUTPUT);  // reset
        pinMode(m_strobePin , OUTPUT);  // strobe
        pinMode(m_linePin   , INPUT);
        resetEQ();
    }

    void readEQ()
    // Function to read 7 band equalizers
    {
         resetEQ();
         for(m_readIndex=0; m_readIndex <7; m_readIndex++)
         {

         delayMicroseconds(30);
         m_spectrum[m_readIndex] = analogRead(m_linePin);
         digitalWrite(m_strobePin,HIGH);
         digitalWrite(m_strobePin,LOW); // strobe pin on the shield - kicks the IC up to the next band
     }
    }

    uint16_t readBand(uint8_t band)
    {
        if(m_readIndex == band)
        {
            m_spectrum[m_readIndex] = analogRead(m_linePin);
            return m_spectrum[m_readIndex];
        }

        if(m_readIndex > band)
            resetEQ();

        for(int i = 0 ; i < band ; i++)
        {
            digitalWrite(m_strobePin,HIGH);
            delayMicroseconds(30);
            digitalWrite(m_strobePin,LOW);
            m_readIndex++;
        }

        delayMicroseconds(30);
        m_spectrum[m_readIndex] = analogRead(m_linePin);
        return m_spectrum[m_readIndex];
    }

protected:
    uint8_t  m_resetPin,m_strobePin,m_linePin;
    uint16_t m_spectrum[7];
    uint8_t  m_readIndex;

    void resetEQ()
    {
        digitalWrite(m_resetPin, HIGH);
        digitalWrite(m_resetPin, LOW);
        digitalWrite(m_strobePin,HIGH);
        digitalWrite(m_strobePin,LOW);
        m_readIndex = 0;
    }
};

#endif // MSGEQ7
