#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include "settings.h"


extern uint16_t lastCycleMS;

enum buttonState
{
    notPressed,
    pressed,
    longPressed,
    keptPressed

};

class pushButton
{
public:
    pushButton(int8_t pinNr, bool inverted = false) :
        m_pinNr(pinNr) , m_inverted(inverted),m_buttonState(notPressed)
    {

    }

    bool isValid()
    {
        return m_pinNr != -1;
    }

    bool read()
    {
        bool status = digitalRead(m_pinNr);
        if(m_inverted) status = !status;
        return status        ;
    }

    bool isPressed()
    {
        if(m_buttonState == pressed)
        {
            m_buttonState = notPressed;
            return true;
        }
        else
            return false;
    }

    bool isLongPressed()
    {
        if(m_buttonState == longPressed)
        {
            m_buttonState = notPressed;
            return true;
        }
        else
            return false;
    }

    bool isKeptPressed()
    {
        return m_buttonState == keptPressed;
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
        bool status = read();

        if(status == m_lastStatus)
        {
            if(status)
            {
                m_pushTime += lastCycleMS;
                if(m_pushTime > m_keptPressThreshold)
                {
                    m_buttonState = keptPressed;
                }
                else if((m_pushTime > m_keptPressThreshold) && !m_longPresed)
                {
                    m_longPresed = true;
                    m_buttonState = longPressed;
                }
            }
            else
                return;
        }
        else
        {
            if(m_pushTime < m_longPressThreshold)
            {
                m_buttonState = pressed;
            }
            else if((m_pushTime > m_longPressThreshold) && !m_longPresed)
            {
                m_buttonState = longPressed;
            }
            else
            {
                m_buttonState = notPressed;
            }
            m_longPresed    = false;
            m_pushTime      = 0;
        }
    }

protected:
    int8_t      m_pinNr;
    bool        m_inverted;
    bool        m_lastStatus;
    uint16_t    m_pushTime;
    bool        m_longPresed;
    buttonState m_buttonState;
    static const uint16_t    m_longPressThreshold   = 3000;
    static const uint16_t    m_keptPressThreshold   = 5000;
};

#endif // PUSHBUTTON_H

