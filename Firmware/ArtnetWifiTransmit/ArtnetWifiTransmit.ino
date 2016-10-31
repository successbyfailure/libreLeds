/*
This example will transmit a universe via Art-Net into the Network.
This example may be copied under the terms of the MIT license, see the LICENSE file for details
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArtnetWifi.h>

//Wifi settings
const char* ssid = "DarkAP-2.4Ghz"; // CHANGE FOR YOUR SETUP
const char* password = "en1lugar2mancha"; // CHANGE FOR YOUR SETUP

// Artnet settings
ArtnetWifi artnet;
const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.
const char host[] = "10.0.1.108"; // CHANGE FOR YOUR SETUP your destination

uint8_t leds = 84;

// connect to wifi – returns true if successful or false if not
boolean ConnectWifi(void)
{
  boolean state = true;
  int i = 0;

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");
  
  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false;
      break;
    }
    i++;
  }
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}

void setup()
{
  Serial.begin(115200);
  ConnectWifi();
  artnet.begin(host);
  artnet.setLength(3*leds);
  artnet.setUniverse(startUniverse);
}

void loop()
{  
    uint8_t index = 0;
    for(int i = 0 ; i < leds ; i++)
    {
        uint16_t byte = 0;
        for ( uint8_t l = 0 ; l < leds ; l++)
        {
            uint8_t val[3] = {0,0,0};
            if(l == index)               val[1] = 150;
            if(l < index && l > index-5) val[1] = 150;
            if(index < 5 && l > leds-index)  val[1] = 150;
            for(int c = 0 ; c < 3 ; c++)
            {
                artnet.setByte(byte++, val[c]);
            }
        }
        artnet.write();
        delay(20);
        index++;
    }
}
