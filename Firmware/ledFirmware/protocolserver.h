#ifndef PROTOCOLSERVER
#define PROTOCOLSERVER

#include "protocol.h"
#include "settings.h"
#include "eepromstorage.h"



extern uint16_t lastCycleMS;
extern uint16_t tcpPackets;
extern uint16_t timeSinceLastFrameMS;


extern wMode currentWifiMode;

static std::vector<String> splitStr(String& str,String sep)
{
        std::vector<String> result;
  if(str.length() < 1) return result;
  int sep_index = str.indexOf(sep);
  if(sep_index == -1)
  {//no hay separador, se devuelve un unico elemento
    result.push_back(str);
    return result;
  }

  while (sep_index >= 0)
  {
    result.push_back(str.substring(0,sep_index));
    str = str.substring(sep_index+sep.length());
    sep_index = str.indexOf(sep);
  }
  if(str.length()) result.push_back(str);
        return result;
}

static int indexOfStringInString(String& str, String value)
{
    uint8_t valSize = value.length();
    char    val[valSize+1];
    value.toCharArray(val,valSize+1);
    //Serial.print("Searching val: ");Serial.print(value);Serial.print(" strLen:");Serial.println(str.length());
    if(str.length() < valSize)
        return -1;

    for(int p = 0 ; p < str.length()-valSize+1 ; p++)
    {
        char    data[valSize+1];
        int i = 0;
        bool found = true;
        for(int c = p ; c < p+valSize ; c++)
        {
            data[i] = str.charAt(c);
            //Serial.print("Char : ");Serial.print(c);Serial.print("  Val: "); Serial.print((uint8_t)data[i]); Serial.print("\tExpected:");Serial.println((uint8_t)val[i]);
            if(val[i] != data[i])
            {
                found = false;
                break;
            }
            i++;
        }
        if(found)
            return p;
    }
    return -1;
}

class protocolClient
{
public:
    protocolClient(settingsStorage* e,WiFiClient c = WiFiClient())
    {
        setSocket(c);
        m_settingsStorage = e;
    }

    bool connectToHost(char* host,uint16_t port)
    {
        return m_socketClient.connect(host,port);
    }

    void setSocket(WiFiClient c)
    {
        m_socketClient = c;
    }

    void setSerialMode()
    {
        m_serialClient = true;
    }

    bool isSerialClient()
    {
        return m_serialClient;
    }

    void update()
    {
        if(isOpen())
        {
            if(m_serialClient)
                readSerial();
            else if(m_socketClient)
                readWifiClient();

            if(m_buffer.length())
                parseBuffer(m_buffer);
        }
    }

    bool isOpen()
    {
        if(m_serialClient)
            return true;
        else if (m_socketClient)
            return m_socketClient.connected();

        return false;
    }

    void sendSimpleMsg(uint16_t msgid)
    {
        simpleProtocolMessage msg;
        msg.msgID = msgid;
        sendSimpleMsg(msg);
    }

    void sendSimpleMsg(simpleProtocolMessage& msg)
    {
        char result[sizeof(simpleProtocolMessage)];
        copyRawBytes((uint8_t *)&msg,(uint8_t*)result,sizeof(simpleProtocolMessage));
        encodeAndSend(result,sizeof(simpleProtocolMessage));
    }

    void sendSettings()
    {
        sendBasicSettingsToClient();
        yield();
        sendExtraSettingsToClient();
    }

    void encodeAndSend(char* msg, uint16_t count)
    {
        String pStart = P_PACKETSTART;
        String pEnd   = P_PACKETEND;
        uint16_t totalBytes = pStart.length() + count + pEnd.length();
        char result[pStart.length()+pEnd.length()+count+1];

        for(int i = 0 ; i < pStart.length() ; i++)
            result[i] = pStart.charAt(i);

        int c = 0;
        for(int i = pStart.length() ; i < count+pStart.length() ; i++)
            result[i] = msg[c++];

        c = 0;
        for(int i = pStart.length() + count ; i < totalBytes  ; i++)
            result[i] = pEnd.charAt(c++);
        result[totalBytes+1] = 0;

        yield();
        if(m_serialClient && isOpen())
        {
            int ships = totalBytes/128;
            int left  = totalBytes%128;
            for(int i = 0 ; i < ships ; i++)
            {
                char* d;
                d = &result[i*128];
                //Serial.write(d,128);
                yield();
            }
            if(left > 0)
            {
                char* d;
                d = &result[ships*128];
                //Serial.write(d,left);
                yield();
            }
        }
        else if(m_socketClient && isOpen())
            m_socketClient.write(result,totalBytes);
    }


protected:
    String            m_buffer;
    WiFiClient        m_socketClient;
    bool              m_serialClient      = false;
    settingsStorage*  m_settingsStorage;

    void readSerial()
    {
        while(Serial.available())
            m_buffer += (char)Serial.read();
    }

    void readWifiClient()
    {
        while(m_socketClient.available())
            m_buffer += (char)m_socketClient.read();
    }

    void parseBuffer(String& buf)
    {
        //Cosas raras pasan con los String cargados de caracteres binarios, hay que volcarlos a mano como bytes,
        // No se puede usar ni el igual ni el subString!!
        if(buf.length() == 0) return;

        Serial.print("parseBuffer() - Data in buffer: ");Serial.print(buf.length());Serial.println("bytes");
//        Serial.print("Buffer:"); Serial.println(buf);

        int s_index =  indexOfStringInString(buf,String(P_PACKETSTART));
        int e_index =  indexOfStringInString(buf,String(P_PACKETEND));
//        Serial.print("startSepIndex:");Serial.print(s_index);
//        Serial.print("  endSepIndex: ");Serial.println(e_index);

        //saneado del buffer
        if(s_index < 0)
        {// si no hay inicio de paquete lo que hay en el buffer tiene que ser basura.
            Serial.print("Garbage on input buffer, cleaning ");Serial.print(buf.length());Serial.println("bytes");
            buf = "";
            return;
        }
        int s_lenght = String(P_PACKETSTART).length();
        int e_lenght = String(P_PACKETEND).length();
        //extraccion de comandos
        while ((s_index >= 0) && (e_index > (s_index+s_lenght))) //Si hay inicio y fin de paquete se extrae el comando.
        {// lo que haya en el buffer hasta el inicio de paquete se descarta(basura)
            String packet;
            for(int i = s_index+s_lenght ; i < e_index ; i++)
            {
                packet += buf.charAt(i);
            }
//            Serial.print("Buffer Had: "); Serial.print(buf.length());Serial.print("bytes, getting from:");Serial.print(s_index+s_lenght);Serial.print(" to: ");Serial.println(e_index);
            String tmp;
            for(int i = e_index+e_lenght ; i < buf.length(); i++ )
                tmp += buf.charAt(i);
            buf = "";
            for(int i = 0 ; i < tmp.length(); i++ )
                buf += tmp.charAt(i);
            parsePacket(packet);
            s_index = indexOfStringInString(buf,String(P_PACKETSTART));
            e_index = indexOfStringInString(buf,String(P_PACKETEND));

            //Serial.print(buf.length()); Serial.println("bytes left on rx buffer\n\n");
            yield();
        }
    }

    void parsePacket(String& str)
    {
        tcpPackets++;
        //Serial.print("ParsePacket:"); Serial.print(str.length()); Serial.println("Bytes");
        if      (str.length() == sizeof(simpleProtocolMessage))
            parseSimpleMsg(str);
        else if (str.length() == sizeof(basicSettings))
            m_settingsStorage->setBasicSettingsBytes(str);
        else if (str.length() == sizeof(extraSettings))
            m_settingsStorage->setExtraSettingsBytes(str);
        else
            parsePacketExtra(str);
    }

    virtual void parsePacketExtra(String& str) {;}

    void parseSimpleMsg(String& str)
    {
        simpleProtocolMessage msg;
        msg.magicNumber = 0;
        copyRawBytes(str,msg);
        if(msg.magicNumber == simpleProtocolMessage().magicNumber)
            Serial.println("... OK!");
        else
        {
            Serial.print("...  FAIL! magic number ="); Serial.println(msg.magicNumber);
            return;
        }
        Serial.print("Procesando Mensaje : ");Serial.println(msg.msgID);

        if(msg.msgID == cmdGetSettings)
        {
            sendSettings();
        }
        else if(msg.msgID == cmdPing)
        {
            sendSimpleMsg(respPong);
        }
        else if(msg.msgID == cmdSaveSettings)
        {
            m_settingsStorage->save();
            sendSettings();
        }
        else if(msg.msgID == cmdReloadSettings)
        {
            m_settingsStorage->load();
            sendSettings();
        }
        else if(msg.msgID == cmdFactorySettings)
        {
            m_settingsStorage->clear();
            m_settingsStorage->load();
            sendSettings();
        }
        else if(msg.msgID == cmdReset)
        {
            //
        }
    }

    void sendBasicSettingsToClient()
    {
        char data[sizeof(basicSettings)];
        m_settingsStorage->getBasicSettingsBytes((uint8_t*)data);
        encodeAndSend(data,sizeof(basicSettings));
    }

    void sendExtraSettingsToClient()
    {
        char data[sizeof(extraSettings)];
        m_settingsStorage->getExtraSettingsBytes((uint8_t*)data);
        encodeAndSend(data,sizeof(extraSettings));
    }
};

class protocolServer
{
public:
    protocolServer(settingsStorage* e) :
        m_settingsStorage(e),
        m_tcpEnabled(e->getBasicSettings().localPortEnabled),
        TCPserver(e->getBasicSettings().localPort)
    {
//        if(e->getBasicSettings().serialClient)
//            addSerialClient();
//esto hay que hacerlo en el heredado!
    }

    void addSerialClient()
    {
        addClient(true);
    }

    void begin()
    {
        if(m_tcpEnabled)
        {
            TCPserver.begin();
        }
    }

    void update()
    {
        if(m_tcpEnabled)
            manageSocketClients();
        if( ((currentWifiMode == wifiClient  ) && (m_settingsStorage->getBasicSettings().autoConnectRemote))
            ||
            ((currentWifiMode == wifiOverride) && (m_settingsStorage->getBasicSettings().overrideAutoConnectRemote)) )
            checkServerConnection();

        for(int i = 0 ; i < m_clients.size() ; i++)
            m_clients[i]->update();
    }

    std::vector<protocolClient*> getClients()
    {
        return m_clients;
    }
    void connectMaster()
    {
        char* remote;
        uint16_t port;

        if((currentWifiMode == wifiClient  ) && (m_settingsStorage->getBasicSettings().autoConnectRemote))
        {
            remote = m_settingsStorage->getBasicSettings().remoteHost;
            port   = m_settingsStorage->getBasicSettings().remotePort;
        }else if((currentWifiMode == wifiOverride) && (m_settingsStorage->getBasicSettings().overrideAutoConnectRemote))
        {
            remote = m_settingsStorage->getBasicSettings().overrideRemoteHost;
            port   = m_settingsStorage->getBasicSettings().overrideRemotePort;
        }

        Serial.print("Connecting to ");Serial.print(remote);
        Serial.print(":");Serial.print(port);
        Serial.print("... ");
        addClient();
        if(m_clients.at(m_clients.size()-1)->connectToHost(remote,port))
            Serial.println(" OK!");
        else
            Serial.println(" FAIL!");
    }
protected:
    std::vector<protocolClient*> m_clients;

    WiFiServer                   TCPserver;
    bool                         m_tcpEnabled;
    uint16_t                     m_offlineTime;
    settingsStorage*             m_settingsStorage;

    virtual void addClient(bool serial = false, WiFiClient c = WiFiClient()) = 0;
    virtual void removeClient(uint8_t index)                                 = 0;


    void checkServerConnection()
    {
        bool tcpConnFound = false;
        for(int i = 0 ; i < m_clients.size() ; i++)
            if(!m_clients.at(i)->isSerialClient())
                tcpConnFound = true;
        if(tcpConnFound)
            m_offlineTime = 0;
        else
        {
            m_offlineTime += timeSinceLastFrameMS;
            if(m_offlineTime > 15000)
            {
                m_offlineTime = 0;
                connectMaster();
            }
        }
    }

    void manageSocketClients()
    {
        for(int i = (m_clients.size() -1) ; i >= 0 ; i--)
        {
            if(!m_clients.at(i)->isOpen())
                removeClient(i);
            if(m_clients.size() == 0)
                Serial.println("Last client left.");
        }

        while(TCPserver.hasClient())
        {
            Serial.println("Incoming tcp client!");
            WiFiClient serverClient = TCPserver.available();
            if(m_clients.size() < MAX_SRV_CLIENTS)
            {
                addClient(false, serverClient);
            }
            else
            {
                serverClient.println("Sorry, I'm full");
                serverClient.stop();
                Serial.print(String(F("D:Client rejected: max_clients")));
            }
        }
    }
};

#endif // PROTOCOLSERVER

