#ifndef PROTOCOLCLIENT_H
#define PROTOCOLCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTime>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


#include "../../Firmware/ledFirmware/settings.h"
#include "../../Firmware/ledFirmware/protocol.h"

class protocolClient : public QObject
{
    Q_OBJECT
public:
    explicit protocolClient(QTcpSocket* socket = 0);
            ~protocolClient();

    void close()
    {
        if      (m_serialEnabled)
            m_serial.close();
        else if (m_socketEnabled)
            m_socket->close();
    }

    bool isOpen()
    {
        if      (m_serialEnabled)
            return false;
        else if (m_socketEnabled)
            return m_socket->isOpen();
        return false;
    }

    bool isValid() {return m_socketEnabled || m_serialEnabled;}
    basicSettings getBasicSettings()    {return m_basicSettings;}
    extraSettings getExtraSettings()    {return m_extraSettings;}

protected:
    basicSettings                   m_basicSettings;
    extraSettings                   m_extraSettings;
    bool                            m_settingsReceived = false;
    QByteArray                      m_buffer;
    QTcpSocket*                     m_socket;
    QSerialPort                     m_serial;
    bool                            m_deleteSocket;
    bool                            m_socketEnabled;
    bool                            m_serialEnabled;

    QTimer      m_pingTimer;
    QTimer      m_pingTimeoutTimer;
    QTime       m_pingElapsedTime;

    void parseBuffer();
    void parsePacket(QByteArray& data);
    void parseSimpleMsg(QByteArray& data);
    void parseDataMsg(QByteArray& data);
    void parseBasicSettings(QByteArray& data);
    void parseExtraSettings(QByteArray& data);

    virtual void parseExtra(QByteArray&)                        {;}    
    virtual void processMessage(simpleProtocolMessage)          {;}
    virtual void processDataReadings(dataReadingsPacket)        {;}
    virtual void initClient()      {sendSimpleMsg(cmdGetSettings);}


    void encodeAndSend(QByteArray& data);
    void writeData(QByteArray& data);

    void deleteSocketIfNecessary()
    {
        if(m_deleteSocket)
        {
            if(m_socket != 0)
                m_socket->deleteLater();
        }
        m_socket = 0;
    }

signals:
    void connecting();
    void connected(bool b = true);
    void ready();
    void disconnected(bool b = true);
    void error();

    void tx();
    void rx();

    void simpleMessage(simpleProtocolMessage msg);
    void dataReadings(dataReadingsPacket packet);
    void basicSettingsReceived(basicSettings s);
    void extraSettingsReceived(extraSettings s);

    void pingTime(quint16 ms);
    void pingTime(QString pingStr);

protected slots:
    void deviceOpened();
    void readTcpData();
    void readSerialData();

    void tcpDisconnected();
    void tcpError(QAbstractSocket::SocketError e);
    void serialClosed();
    void serialError(QSerialPort::SerialPortError);
    void socketDied();

    void ping();
    void pong();
    void pingTimeout();
    void startPing();
    void stopPing();

public slots:
    void setTcpSocket(QTcpSocket* s, bool deleteSocket = true);
    void connectToHost(QString host, quint16 port);
    void OpenSerial(QSerialPortInfo device, quint32 bauds);
    void sendSimpleMsg(simpleProtocolMessage msg);
    void sendSimpleMsg(quint16 msgID);
    void sendBasicSettings(basicSettings s);
    void sendExtraSettings(extraSettings s);
    void saveSettings();
    void reloadSettings();
    void reset();
    void factoryReset();
    void getSettings() {sendSimpleMsg(cmdGetSettings);}
};

#endif // PROTOCOLCLIENT_H
