#include "ledcontroller.h"

ledController::ledController(QTcpSocket *tcp) : protocolClient(tcp)
{
    qDebug() << "Creating ledcontroller";
}

ledController::~ledController()
{

}

void ledController::sendLedGadgetProtocol(ledGadgetProtocolPacket p)
{
    QByteArray data;
    uint16_t size = sizeof(ledGadgetProtocolPacket);
    char* c = (char*)&p;
    for(uint16_t i = 0 ; i < size ; i++)
    {
        data.append(c[i]);
    }
    encodeAndSend(data);
}

void ledController::getAnimations()
{
    ledGadgetProtocolPacket p;
    p.cmd = cmdGetAnimationList;
    sendLedGadgetProtocol(p);
}

void ledController::parseLedGadgetProtocol(QByteArray& data)
{
    ledGadgetProtocolPacket p;
    char* bytes = (char*)&p;
    p.magicNumer = 0;
    uint16_t size = sizeof(ledGadgetProtocolPacket);

    for(uint16_t i = 0 ; i < size ; i++)
    {
        bytes[i] = data.at(i);
    }

    if(p.magicNumer == ledGadgetProtocolPacket().magicNumer)
        emit ledGadgetProtocol(p);
}

void ledController::parseAnimationList(QByteArray& data)
{
    ledGadgetAnimationListPacket p;
    char* bytes = (char*)&p;
    p.magicNumer = 0;
    uint16_t size = sizeof(ledGadgetAnimationListPacket);

    for(uint16_t i = 0 ; i < size ; i++)
    {
        bytes[i] = data.at(i);
    }

    if(p.magicNumer == ledGadgetAnimationListPacket().magicNumer)
        emit animationList(p);
}

void ledController::parseExtra(QByteArray& data)
{
    if(data.size() == sizeof(ledGadgetAnimationListPacket))
        parseAnimationList(data);
    else if(data.size() == sizeof(ledGadgetProtocolPacket))
        parseLedGadgetProtocol(data);
}


void ledController::processDataReadings(dataReadingsPacket data)
{

}
