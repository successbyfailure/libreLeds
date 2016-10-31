#ifndef ESPWIFI
#define ESPWIFI

#include "eepromstorage.h"
#include "qtcompat.h"

ESP8266WebServer updateServer(8080);
const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
bool wifiAPMode = false;

//Firmware updater
void startHttpUpdater(uint16_t port)
{
    updateServer = ESP8266WebServer(port);
    updateServer.on("/", HTTP_GET, [](){
      updateServer.sendHeader("Connection", "close");
      updateServer.sendHeader("Access-Control-Allow-Origin", "*");
      updateServer.send(200, "text/html", serverIndex);
    });
    updateServer.on("/update", HTTP_POST, [](){
      updateServer.sendHeader("Connection", "close");
      updateServer.sendHeader("Access-Control-Allow-Origin", "*");
      updateServer.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
      ESP.restart();
    },[](){
      HTTPUpload& upload = updateServer.upload();
      if(upload.status == UPLOAD_FILE_START){
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)){//start with max available size
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_WRITE){
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_END){
        if(Update.end(true)){ //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      yield();
    });
    updateServer.begin();
}

void wifi_init(settingsStorage* e)
{
    basicSettings s = e->getBasicSettings();
//    WiFi.mode(WIFI_STA);
//    WiFi.disconnect();
    bool overmindFound = false;
    if(s.allowWifiOverride)
    {
        int n = WiFi.scanNetworks();
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; i++)
        {
            if(WiFi.SSID(i) == s.wifiOverrideESSID)
            {
                Serial.print("Overmind found, joining..");
                Serial.print(WiFi.SSID(i));
                Serial.print(" - ");
                Serial.println(s.wifiOverridePasswd);
                WiFi.softAPdisconnect(true);
                yield();
                WiFi.mode(WIFI_STA);
                WiFi.begin(s.wifiOverrideESSID, s.wifiOverridePasswd);
                yield();
                overmindFound = true;
    }}}

  if(overmindFound)
  {
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }

      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
  }
  else if(s.wifiMode == wifiAP)
  {
      Serial.print("AP_MODE:");
      Serial.print(s.wifiESSID);
      Serial.print(" - ");
      Serial.println(s.wifiPasswd);
      WiFi.disconnect();
      yield();
      WiFi.softAP(s.wifiESSID, s.wifiPasswd);
      wifiAPMode = true;
      yield();
  } else if(s.wifiMode == wifiClient) {
      Serial.print("WIFI_CLIENT_MODE:");
      Serial.print(s.wifiESSID);
      Serial.print(" - ");
      Serial.println(s.wifiPasswd);
      WiFi.softAPdisconnect(true);
      yield();
      if(s.staticIP)
      {
          IPAddress ip     (s.IP[0]    ,s.IP[1]    ,s.IP[2]    ,s.IP[3]);
          IPAddress gateway(0          ,0          ,0          , 0);
          IPAddress subnet (s.subnet[0],s.subnet[1],s.subnet[2],s.subnet[3]);
          WiFi.config      (ip, gateway, subnet);
      }
      WiFi.mode(WIFI_STA);
      WiFi.begin(s.wifiESSID, s.wifiPasswd);
      yield();
      uint8_t i = 0;
      while ( (WiFi.status() != WL_CONNECTED) && (i < 20) )
      {
        delay(500);
        Serial.print(".");
        i++;
      }
      if(WiFi.status() != WL_CONNECTED)
          Serial.println("Not Connected!");
      else
          Serial.println("Connected!");
  } else if(s.wifiMode == wifiMesh) {

  }

    IPAddress ip;
    String localIP;
    if(wifiAPMode)
        ip = WiFi.softAPIP();
    else
        ip = WiFi.localIP();
    localIP += ip[0];
    localIP += ".";
    localIP += ip[1];
    localIP += ".";
    localIP += ip[2];
    localIP += ".";
    localIP += ip[3];

    if(e->getBasicSettings().MdnsEnabled)
    {
        if(MDNS.begin(s.id))
        {
            Serial.print("MDNS started, name:");Serial.print(s.id);Serial.println(".local");
            localIP = "";
            localIP += s.id;
            localIP += ".local";
        }
        else
        {
            Serial.println("Failed to start MDNS");
        }
    }

    if(e->getBasicSettings().httpUpdaterEnabled)
    {
        MDNS.addService("http", "tcp", e->getBasicSettings().httpUpdatePort);
        startHttpUpdater(e->getBasicSettings().httpUpdatePort);
        Serial.print("Updater ready, Open http://");
        Serial.print(localIP);
        Serial.print(":");
        Serial.print(e->getBasicSettings().httpUpdatePort);
        Serial.println(" to upload hex");
    }
}


#endif // ESPWIFI

