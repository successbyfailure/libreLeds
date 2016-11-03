#ifndef LEDGADGETCLIENT
#define LEDGADGETCLIENT

#include "protocolserver.h"
#include "ledgadget.h"

class ledGadgetClient : public protocolClient
{
public:
    ledGadgetClient(ledGadget* l, settingsStorage* e, WiFiClient c = WiFiClient()) :
        protocolClient(e,c),m_ledGadget(l) {;}


    void sendLedGadgetProtocolPacket(ledGadgetProtocolPacket& p)
    {
        uint16_t size = sizeof(ledGadgetProtocolPacket);
        char bytes[size];
        copyRawBytes((uint8_t*)&p,(uint8_t*)bytes,size);
        encodeAndSend(bytes,size);
    }

    void sendLedGadgetAnimationListPacket(ledGadgetAnimationListPacket& p)
    {
        uint16_t size = sizeof(ledGadgetAnimationListPacket);
        char bytes[size];
        copyRawBytes((uint8_t*)&p,(uint8_t*)bytes,size);
        encodeAndSend(bytes,size);
    }

    void sendAnimationList()
    {
        std::vector<ledGadgetAnimations> items = m_ledGadget->getAnimations();
        ledGadgetAnimationListPacket resp;
        for(int i = 0 ; i < items.size() ; i++)
        {
            resp.animationID[resp.usedEntries] = items.at(i);
            animation2name(items.at(i)).toCharArray(resp.animationName[resp.usedEntries],ANIMATIONLIST_MAXSTR);
            resp.usedEntries++;
            if(resp.usedEntries == ANIMATIONLIST_MAXITEMS)
            {
                sendLedGadgetAnimationListPacket(resp);
                resp = ledGadgetAnimationListPacket();
            }
        }
        if(resp.usedEntries > 0)
            sendLedGadgetAnimationListPacket(resp);
    }

    void parsePacketExtra(String &str)
    {
        if(str.length() == sizeof(ledGadgetProtocolPacket))
            parseLedProtocolPacket(str);
    }

protected:
    ledGadget*  m_ledGadget;

    void parseLedProtocolPacket(String& str)
    {
        ledGadgetProtocolPacket p;
        p.magicNumer = 0;
        copyRawBytes(str,(uint8_t*)&p,sizeof(ledGadgetProtocolPacket));
        if(p.magicNumer != ledGadgetProtocolPacket().magicNumer)
        {
            Serial.println("ledProtocolPacket: Wrong magic number!");
            return;
        }

        if      (p.cmd == cmdGetAnimationList)
            sendAnimationList();
        else if (p.cmd == cmdSetAnimation)
            m_ledGadget->setAnimation(p);
        else if (p.cmd == cmdOff)
            m_ledGadget->off();
        else if (p.cmd == cmdSetBrigth)
            m_ledGadget->setBrigthness(p.floatVal0);
        else if (p.cmd == cmdDimm)
            m_ledGadget->dimm(p.floatVal0);
    }

};


class ledGadGetServer : public protocolServer
{
public:
    ledGadGetServer(settingsStorage* e) :
        protocolServer(e)
    {
        if(e->getBasicSettings().serialClient)
        addSerialClient();
    }

    void setLedGadget(ledGadget* l)
    {
        m_ledGadget = l;
    }

protected:
    std::vector<ledGadgetClient> m_ledClients;
    ledGadget*                   m_ledGadget;

    void addClient(bool serial = false, WiFiClient c = WiFiClient())
    {
        Serial.println("adding ledgadget client.");
        m_ledClients.push_back(ledGadgetClient(m_ledGadget,m_settingsStorage,c));
        m_clients.push_back(&m_ledClients.at(m_ledClients.size()-1));
        if(serial)
            m_clients.at(m_clients.size()-1)->setSerialMode();
    }

    void removeClient(uint8_t index)
    {
        Serial.print("Client disconnected"); Serial.println(index);
        m_clients.erase(m_clients.begin()+index);
        m_ledClients.erase(m_ledClients.begin()+index);
    }

};

#endif // LEDGADGETCLIENT
