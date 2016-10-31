#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QObject>
#include <QTcpSocket>

class socketClient : public QTcpSocket
{
    Q_OBJECT
public:
    socketClient();

signals:
    void tx();
    void rx();

    void sensorRead(float gx,float gy,float gz, float accX,float accY,float accZ,float angleY,float rangleY,float Temp, int vuLevel);
protected slots:
    void readData();
    void sendData(QString data);
public slots:
    void off();
    void setColor(quint8 r, quint8 g, quint8 b);
    void setBrightness(float b);
    void prevEffect();
    void nextEffect();
    void animationCircle();
    void animationDoubleCircle();
    void animationRainbow();
    void animationFlash();
    void setOffset(int value);
    void requestSensorData(bool);
};

#endif // SOCKETCLIENT_H
