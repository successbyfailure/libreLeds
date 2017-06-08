#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <QTimer>
#include <QTime>
#include <QColor>

#include "protocolclient.h"


class ledController : public protocolClient
{
    Q_OBJECT
public:
    explicit ledController(QTcpSocket* tcp = 0);
    ~ledController();
protected:

    void parseExtra(QByteArray& data);
    void parseLedGadgetProtocol(QByteArray& data);
    void parseAnimationList(QByteArray& data);
    void processDataReadings(dataReadingsPacket data);

signals:
void ledGadgetProtocol(ledGadgetProtocolPacket);
void animationList(ledGadgetAnimationListPacket);

public slots:
    void getAnimations();
    void sendLedGadgetProtocol(ledGadgetProtocolPacket p);

    void flash()
    {
        ledGadgetProtocolPacket p;
        p.cmd = cmdSetAnimation;
        p.intVal0 = animationFlash;
        sendLedGadgetProtocol(p);
    }

    void fade()
    {
        ledGadgetProtocolPacket p;
        p.cmd = cmdSetAnimation;
        p.intVal0 = animationFade;
        sendLedGadgetProtocol(p);
    }

    void setColor(QColor c)
    {
        ledGadgetProtocolPacket p;
        p.cmd = cmdSetColor;
        p.intVal0 = c.red();
        p.intVal1 = c.green();
        p.intVal2 = c.blue();
        sendLedGadgetProtocol(p);
    }

    void setBrightness(float f)
    {
        setBrightness(f*255);
    }

    void setBrightness(quint8 b)
    {
        ledGadgetProtocolPacket p;
        p.cmd = cmdSetBright;
        p.intVal0 = b;
        sendLedGadgetProtocol(p);
    }
};

#endif // LEDCONTROLLER_H
