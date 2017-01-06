#ifndef MSGEQ7CLASS
#define MSGEQ7CLASS

#include "settings.h"

extern uint16_t timeSinceLastFrameMS;
extern uint16_t lastCycleMS;
extern uint16_t thisCycleMS;

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
        for(uint8_t b = 0 ; b < 7 ; b++)
            m_eqHistory.push_back(std::vector<uint8_t>());
        resetEQ();
        m_peaking = false;
        m_peakHappened = false;
        m_bpms    = 135;
    }

    void analize()
    {
        m_lastPeakMs += timeSinceLastFrameMS;
        readEQ();
        analizeBpms();
    }

    bool peakHappened()
    {
        if(m_peakHappened)
        {
            m_peakHappened = false;
            return true;
        }
        else
            return false;
    }

    uint8_t                             vuLevel()   {return m_vuLevel;}
    uint8_t*                            spectrum()  {return m_spectrum;}
    float                               bpms()      {return m_bpms;}
    std::vector<uint8_t>                vuHistory() {return m_vuHistory;}
    std::vector<std::vector<uint8_t> >  eqHistory() {return m_eqHistory;}

protected:
    uint8_t  m_resetPin,m_strobePin,m_linePin;
    uint8_t  m_spectrum[7];
    uint8_t  m_readIndex;
    uint8_t  m_vuLevel;

    float    m_bpms      ;
    uint16_t m_lastPeakMs;
    bool     m_peaking   ;
    bool     m_peakHappened;

    std::vector<float>                  m_BpmHistory;
    std::vector<uint8_t>                m_vuHistory;
    std::vector<std::vector<uint8_t> >  m_eqHistory;

    void resetEQ()
    {
        digitalWrite(m_resetPin ,HIGH);
        digitalWrite(m_resetPin ,LOW);
        digitalWrite(m_strobePin,HIGH);
        digitalWrite(m_strobePin,LOW);
        m_readIndex = 0;
    }

    void readEQ()
    // Function to read 7 band equalizers
    {
        uint16_t vuLevel = 0;
        resetEQ();
        for(m_readIndex=0;m_readIndex <7; m_readIndex++)
        {
            delayMicroseconds(30);
            uint16_t val = analogRead(m_linePin);
            if(val < 128)
                val = 0;
            else
                val -=128;

            m_spectrum[m_readIndex] = val*100/896;
            vuLevel += m_spectrum[m_readIndex];
            digitalWrite(m_strobePin,HIGH);
            digitalWrite(m_strobePin,LOW); // strobe pin on the shield - kicks the IC up to the next band
        }
        m_vuLevel = vuLevel*2/7;

        m_vuHistory.push_back(m_vuLevel);
        if(m_vuHistory.size() > 50)
            m_vuHistory.erase(m_vuHistory.begin());

        for(uint8_t b = 0 ; b < 7 ; b++)
            m_eqHistory[b].push_back(m_spectrum[b]);
        if(m_eqHistory[0].size() > 15)
            for(uint8_t b = 0 ; b < 7 ; b++)
                m_eqHistory[b].erase(m_eqHistory[b].begin());
    }

    void analizeBpms()
    {
        for(uint8_t b = 0 ; b < 7 ; b++)
        {
            uint8_t maxPeakValue = 0;
            for(uint8_t i = 0 ; i < m_vuHistory.size() ; i++)
                if(m_vuHistory[i] > maxPeakValue) maxPeakValue = m_vuHistory[i];

            uint16_t expectedPeakMS = 60/m_bpms*1000;
            if( m_lastPeakMs > (expectedPeakMS*1.8) )
            {
                m_lastPeakMs = m_lastPeakMs%expectedPeakMS;
            }

            if(m_peaking)
            {
                if(m_vuLevel < (maxPeakValue*0.6) )
                    m_peaking = false;
            }
            else
            {
                if( (m_vuLevel > (maxPeakValue*0.80) ) &&
                  ( (m_vuHistory[m_vuHistory.size()-1] / m_vuLevel) > 0.5f) )
                {
                    m_peaking = true;
                    yield();
                    float bpms = (1000.0f/(m_lastPeakMs+thisCycleMS))*60.0f;
                    if( (bpms > 80.0f) && (bpms < 170.0f) )
                    {
                        m_BpmHistory.push_back(bpms);
                        if(m_BpmHistory.size() > 15)
                            m_BpmHistory.erase(m_BpmHistory.begin());
                        float b = 0;
                        for(uint8_t i = 0 ; i < m_BpmHistory.size() ; i++)
                        {
                            b+=m_BpmHistory[i];
                        }
                        m_bpms = b/m_BpmHistory.size();
                        m_lastPeakMs = 0;
                        m_peakHappened = true;
                    }
                }
            }
        }
    }
};

#endif // MSGEQ7

