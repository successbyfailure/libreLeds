#include "protocolclient.h"

protocolClient::protocolClient(QTcpSocket *socket) : QObject(socket)
{
    qDebug() << "Creating protocol client";
    m_socket = 0;
    m_serialEnabled = false;

    connect(&m_pingTimer        ,SIGNAL(timeout()),SLOT(ping()));
    connect(&m_pingTimeoutTimer ,SIGNAL(timeout()),SLOT(pingTimeout()));
    connect(this,SIGNAL(connected()),this,SLOT(startPing()));
    connect(this,SIGNAL(disconnected()),this,SLOT(stopPing()));
    connect(&m_initDelayTimer,SIGNAL(timeout()),this,SLOT(deviceOpened()));

    if(socket != 0)
        setTcpSocket(socket);
    else
        m_socketEnabled = false;
}

protocolClient::~protocolClient()
{
    deleteSocketIfNecessary();
}

void protocolClient::connectToHost(QString host, quint16 port)
{
    if(isOpen())
    {
        qDebug() << "Already open, Close First!";
        return;
    }
    m_serialEnabled = false;
    m_socketEnabled = true;
    emit connecting();
    deleteSocketIfNecessary();
    setTcpSocket(new QTcpSocket(),true);
    m_socket->connectToHost(host,port);
}

void protocolClient::setTcpSocket(QTcpSocket *s, bool deleteSocket)
{
    deleteSocketIfNecessary();
    m_deleteSocket = deleteSocket;
    m_socket = s;
    m_socketEnabled = true;
    disconnect(s,SIGNAL(destroyed(QObject*)),this,SLOT(socketDied()));
    connect(s,SIGNAL(readyRead()),this,SLOT(readTcpData()));
    connect(s,SIGNAL(connected()),this,SLOT(deviceOpened()));
    connect(s,SIGNAL(disconnected()),this,SLOT(tcpDisconnected()));
    connect(s,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(tcpError(QAbstractSocket::SocketError)));

    if(s->isOpen())
    {
        qDebug() << "Delaying init 500ms";
        m_initDelayTimer.setSingleShot(true);
        m_initDelayTimer.start(500);
    }
}

void protocolClient::OpenSerial(QSerialPortInfo device, quint32 bauds)
{
    if(isOpen())
    {
        qDebug() << "Already open, Close First!";
        return;
    }
    m_serialEnabled = true;
    m_socketEnabled = false;
    emit connecting();
    m_serial.setBaudRate(bauds);
    m_serial.setPort(device);
    if(m_serial.open(QIODevice::ReadWrite))
    {
        disconnect(&m_serial,SIGNAL(readyRead()),this,SLOT(readSerialData()));
        disconnect(&m_serial,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(serialError(QSerialPort::SerialPortError)));
        connect(&m_serial,SIGNAL(readyRead()),this,SLOT(readSerialData()));
        connect(&m_serial,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(serialError(QSerialPort::SerialPortError)));
        deviceOpened();
    }
    else
    {
        emit disconnected();
    }
}

void protocolClient::deviceOpened()
{
    qDebug() << "Device Opened! emit protocolClient::connected()";
    emit connected();
    initClient();
}

void protocolClient::readTcpData()
{
    m_buffer.append(m_socket->readAll());
    emit rx();
    parseBuffer();
}

void protocolClient::readSerialData()
{
    m_buffer.append(m_serial.readAll());
    emit rx();
    parseBuffer();
}

void protocolClient::tcpDisconnected()
{
    emit disconnected();
}

void protocolClient::serialClosed()
{
    emit disconnected();
}

void protocolClient::tcpError(QAbstractSocket::SocketError e)
{
    qDebug() << "tcp error : " << e;
    m_socket->close();
    emit disconnected();
    emit error();
}

void protocolClient::serialError(QSerialPort::SerialPortError e)
{
//    if(e == QSerialPort::SerialPortError::NoError)
//        return;
    qDebug() << "serial error : " << e;
    emit disconnected();
    emit error();
}

void protocolClient::encodeAndSend(QByteArray& data)
{
    QByteArray result;
    result += P_PACKETSTART;
    for(int i = 0 ; i < data.size() ; i++)
        result += data.at(i);
    result += P_PACKETEND;
    writeData(result);
}

void protocolClient::sendSimpleMsg(quint16 msgID)
{
    simpleProtocolMessage msg;
    msg.msgID = msgID;
    sendSimpleMsg(msg);
}

void protocolClient::sendSimpleMsg(simpleProtocolMessage msg)
{
    QByteArray result;
    char* bytes = (char*)&msg;
    for(uint16_t i = 0 ; i < sizeof(simpleProtocolMessage) ; i++)
    {
        result.append(bytes[i]);
    }
    encodeAndSend(result);
}

void protocolClient::writeData(QByteArray& data)
{
    if      (m_socketEnabled && m_socket->isOpen())
    {
        m_socket->write(data);
    }
    //else if (m_serialEnabled && m_serial.isOpen())
    else if (m_serialEnabled && m_serial.isOpen())
    {
        m_serial.write(data);
    }
    else
    {
        qDebug() << "Cannot write data, no device available";
        return;
    }
    emit tx();
}

void protocolClient::parseBuffer()
{
    //qDebug() << "Buffer:" << m_buffer;
    //qDebug() << "-PacketSeparators:" << P_PACKETSTART << P_PACKETSEPARATOR << P_PACKETTERMINATOR;
    if(m_buffer.length() == 0) return;
    int s_index = m_buffer.indexOf(P_PACKETSTART);
    int e_index = m_buffer.indexOf(P_PACKETEND);
    //saneado del buffer
    //qDebug() << "indexs:" << s_index << "indexe" << e_index;
    if(s_index < 0)
    {// si no hay inicio de paquete lo que hay en el buffer tiene que ser basura.
        qDebug() << "Limpiando Buffer";
        m_buffer.clear();
        return;
    }
    //extraccion de comandos
    int packetStartLength = QString(P_PACKETSTART).size();
    int packetEndLength   = QString(P_PACKETEND).size();
    while ((s_index >= 0) && (e_index >= 0)) //Si hay inicio y fin de paquete se extrae el comando.
    {// lo que haya en el buffer hasta el inicio de paquete se descarta(basura)
        QByteArray packet = m_buffer.mid(s_index+packetStartLength,e_index-s_index-packetStartLength);
        parsePacket(packet);
        m_buffer = m_buffer.mid(e_index+packetEndLength);
        s_index  = m_buffer.indexOf(P_PACKETSTART);
        e_index  = m_buffer.indexOf(P_PACKETEND);
    }
    //qDebug() << "Buffer:End" << m_rxBuffer;
}

void protocolClient::parsePacket(QByteArray &data)
{
    //qDebug() <<"Packet Received:" << data.size() << "bytes";
    if      (data.length() == sizeof(simpleProtocolMessage))
        parseSimpleMsg(data);
    else if (data.length() == sizeof(dataReadingsPacket))
        parseDataMsg(data);
    else if (data.length() == sizeof(basicSettings))
        parseBasicSettings(data);
    else if (data.length() == sizeof(extraSettings))
        parseExtraSettings(data);
    else
        parseExtra(data);
}

void protocolClient::parseSimpleMsg(QByteArray& data)
{
    simpleProtocolMessage msg;
    msg.magicNumber = 0;
    copyRawBytes((uint8_t*)data.data(),(uint8_t*)&msg,sizeof(simpleProtocolMessage));
    if(msg.magicNumber == simpleProtocolMessage().magicNumber)
    {
        emit simpleMessage(msg);
        qDebug() << "msgReceived: " << msg.msgID;
        if(msg.msgID == respPong)
            pong();
    }
    else
        qDebug() << "Bad packet: " << msg.magicNumber;

}

void protocolClient::parseDataMsg(QByteArray& data)
{
    dataReadingsPacket result;
    result.magicNumber = 0;
    char * byteStorage = (char *)&result;
    for (size_t i = 0; i < sizeof(dataReadingsPacket) ; i++)
      byteStorage[i] = data.at(i);

    if(result.magicNumber == dataReadingsPacket().magicNumber)
    {
        emit dataReadings(result);
        processDataReadings(result);
    }
    else
        qDebug() << "Bad packet: " << result.magicNumber;
}

void protocolClient::parseBasicSettings(QByteArray& data)
{
    basicSettings result;
    result.magicNumber = 0;
    char * byteStorage = (char *)&result;
    for (size_t i = 0; i < sizeof(basicSettings) ; i++)
      byteStorage[i] = data.at(i);

    if(result.magicNumber == basicSettings().magicNumber)
    {
        qDebug() << "Basic Settings received";
        m_basicSettingsReceived = true;
        emit basicSettingsReceived(result);
        m_basicSettings = result;
        if(m_basicSettingsReceived && m_extraSettingsReceived)
            emit ready();
    }
    else
    {
        qDebug() << "Bad settings packet:" << result.magicNumber;
    }
}

void protocolClient::parseExtraSettings(QByteArray& data)
{
    extraSettings result;
    result.magicNumber = 0;
    char * byteStorage = (char *)&result;
    for (size_t i = 0; i < sizeof(extraSettings) ; i++)
      byteStorage[i] = data.at(i);

    if(result.magicNumber == extraSettings().magicNumber)
    {
        qDebug() << "Extra Settings received";
        m_extraSettingsReceived = true;
        emit extraSettingsReceived(result);
        m_extraSettings = result;
        if(m_basicSettingsReceived && m_extraSettingsReceived)
            emit ready();
    }
    else
    {
        qDebug() << "Bad settings packet:" << result.magicNumber;
    }
}

void protocolClient::socketDied()
{
    m_socket = 0;
    m_socketEnabled = false;
}

void protocolClient::sendBasicSettings(basicSettings s)
{
    QByteArray result;
    char* bytes = (char*)&s;
    for(uint16_t i = 0 ; i < sizeof(basicSettings) ; i++)
    {
        result.append(bytes[i]);
    }
    encodeAndSend(result);
}

void protocolClient::sendExtraSettings(extraSettings s)
{
    QByteArray result;
    char* bytes = (char*)&s;
    for(uint16_t i = 0 ; i < sizeof(extraSettings) ; i++)
    {
        result.append(bytes[i]);
    }
    encodeAndSend(result);
}

void protocolClient::saveSettings()
{
    sendSimpleMsg(cmdSaveSettings);
}

void protocolClient::reloadSettings()
{
    sendSimpleMsg(cmdReloadSettings);
}

void protocolClient::factoryReset()
{
    sendSimpleMsg(cmdFactorySettings);
}

void protocolClient::reset()
{
    sendSimpleMsg(cmdReset);
}

void protocolClient::ping()
{
    sendSimpleMsg(cmdPing);
    m_pingTimer.start();
    m_pingElapsedTime.start();
}

void protocolClient::pong()
{
    quint16 pingms = m_pingElapsedTime.elapsed();
    QString str;
    str += "Ping Time : ";
    str += QString::number(pingms);
    str += "ms";

    m_pingTimer.stop();
    m_pingTimer.start();
    if(m_basicSettingsReceived)
    {
        m_pingTimeoutTimer.stop();
        m_pingTimeoutTimer.start();
    }
    emit pingTime(pingms);
    emit pingTime(str);
    qDebug() << str;
}

void protocolClient::pingTimeout()
{
    stopPing();
    qDebug() << "ping timeout!";
    emit disconnected();
    close();
}

void protocolClient::startPing()
{
    m_pingTimer.start(2500);
    m_pingTimeoutTimer.start(6000);
    ping();
}

void protocolClient::stopPing()
{
    m_pingTimer.stop();
    m_pingTimeoutTimer.stop();
}
