#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include "settings.h"


extern uint16_t lastCycleMS;

class pushButton
{
public:
    pushButton(int8_t pinNr, bool inverted = false) :
        m_pinNr(pinNr) , m_inverted(inverted)
    {

    }

    bool isValid()
    {
        return m_pinNr != -1;
    }

    bool isPressed()
    {
        bool status = digitalRead(m_pinNr);
        if(m_inverted) status = !status;
        return status        ;
    }

    void setup()
    {
        if(isValid())
        {
            pinMode(m_pinNr,INPUT);
            m_lastStatus = digitalRead(m_pinNr);
            if(m_inverted) m_lastStatus = !m_lastStatus;
        }
    }

    void update()
    {
        bool status = digitalRead(m_pinNr);
        if(m_inverted) status = !status;

        if(status == m_lastStatus)
        {
            m_switchStateTime = 0;
            if(status)
                m_pushTime += lastCycleMS;
            else
                return;
        }
        else
        {
            m_switchStateTime += lastCycleMS;
            if(m_switchStateTime > m_switchStateThreshold)
            {
                m_lastStatus = status;
                m_switchStateTime = 0;
                if(!status)
                {
                    if(m_pushTime < m_longPressThreshold)
                    {
                        Serial.print("Button Press:");Serial.print();Serial.print(m_pushTime);Serial.println("ms");
                    }
                    else
                    {
                        Serial.print("Button Long-Press:");Serial.print();Serial.print(m_pushTime);Serial.println("ms");
                    }
                    m_pushTime = 0;
                }
            }
        }
    }

protected:
    int8_t      m_pinNr;
    bool        m_inverted;
    bool        m_lastStatus;
    uint16_t    m_pushTime;
    uint16_t    m_switchStateTime;

    static const uint16_t    m_longPressThreshold   = 3000;
    static const uint16_t    m_switchStateThreshold = 50;
};

#endif // PUSHBUTTON_H

