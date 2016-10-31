#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <QTimer>
#include <QTime>

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
};

#endif // LEDCONTROLLER_H
