#ifndef STORAGESS
#define STORAGESS

#include "aux.h"
#define JSONCONFIGSIZE  1000

#define NODE_ID      F("i")
#define NODE_TYPE    F("t")

#define WIFI_MODE    F("wm")
#define WIFI_ESSID   F("we")
#define WIFI_PWD     F("wp")

#define STATIC_IP_EN F("se")
#define STATIC_IP    F("ip")

#define MQTT_ENABLED F("me")
#define MQTT_SERVER  F("ms")
#define MQTT_PORT    F("mp")

#define ARTNET_ENABLED  F("ae")
#define ARTNET_UNIVERSE F("au")
#define ARTNET_CHANNEL  F("ac")
#define ARTNET_ANNOUNCE F("aa")

#define SACN_ENABLED    F("se")
#define SACN_UNIVERSE   F("su")
#define SACN_CHANNEL    F("sc")

#define STATUS_PIN      F("sp")

#define LED_HW            F("lw")
#define LED_COUNT         F("lc")
#define LED_BRIGHT        F("lb")
#define LED_MAXBRIGHT     F("lm")
#define UNDERVOLT_PROTECT F("up")
#define POWER_CALIBRATION F("pc")

#define LOOP_DELAY     F("ld")
#define SENSOR_REFRESH F("sr")
#define LED_REFRESH    F("lr")
#define NODE_REFRESH   F("nr")
#define OTHER_REFRESH  F("or")
#define WD_REFRESH     F("wd")

#define CONFIG_VERSION F("CV")

class storage
{
public:
  storage()
  {
  }

String stockConfig()
{  ///Pasar toda esta mierda a defines para ahorrar memo
  StaticJsonBuffer<JSONCONFIGSIZE> data;
  JsonObject& root = data.createObject();

  root[NODE_ID]          = String(F("node-"))+String(ESP.getChipId(), HEX);
//  root[NODE_ID]          = "LedMatrix";
  root[NODE_TYPE]        = F("unknown");

  root[WIFI_MODE]        = F("client");
  root[WIFI_ESSID]       = F("-----------------");
  root[WIFI_PWD]         = F("-----------------");

  root[STATIC_IP_EN]     = F("false");
  root[STATIC_IP]        = F("192.168.10.100");

  root[MQTT_ENABLED]     = F("true");
  root[MQTT_SERVER]      = F("192.168.10.10");
  root[MQTT_PORT]        = F("1883");

  root[ARTNET_ENABLED]   = F("true");
  root[ARTNET_UNIVERSE]  = F("1");
  root[ARTNET_CHANNEL]   = F("1");
  root[ARTNET_ANNOUNCE]  = F("true");

  root[SACN_ENABLED]     = F("true");
  root[SACN_UNIVERSE]    = F("1");
  root[SACN_CHANNEL]     = F("1");

  root[STATUS_PIN]       = D4;
  root[LED_HW]           = F("ws2812Strip");
  root[LED_COUNT]        = F("50");
  root[LED_BRIGHT]       = F("1");
  root[LED_MAXBRIGHT]    = F("50");
  root[UNDERVOLT_PROTECT]= F("true");
  root[POWER_CALIBRATION]= F("true");

  root[LOOP_DELAY]       = F("1");
  root[SENSOR_REFRESH]   = F("10");
  root[LED_REFRESH]      = F("15");
  root[NODE_REFRESH]     = F("100");
  root[OTHER_REFRESH]    = F("250");
  root[WD_REFRESH]       = F("5000");

  root[CONFIG_VERSION]   = F("3");

  String result;
  root.printTo(result);
  return result;
}

virtual void    initConfig()                        {;}
virtual void    dumpConfig()                        {;}
virtual String  readFile(String file)               {return "";}
virtual void    writeFile(String file,String& data) {;}
virtual bool    hasStoredConfig()                   {return false;}
virtual String  getKeyValue(String key, String filename = "/config.json")               = 0;
virtual bool    setKeyValue(String key,String value, String filename = "/config.json")  = 0;

protected:




};


class spifsStorage : public storage
{
public:
  spifsStorage() : storage()
  {
    Serial.print(F("Opening SPIFFS..."));
    if(SPIFFS.begin())
    {
      Serial.println(F("OK!"));
      Dir dir = SPIFFS.openDir("/");
      while (dir.next()) {
        String fileName = dir.fileName();
        size_t fileSize = dir.fileSize();
        Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
      }
      Serial.printf("\n");
    }
    else
    {
        Serial.println(F("\n SPIFFS FAIL!"));
    }

    if(!hasStoredConfig())
    {
      initConfig();
    }
    dumpConfig();
  }

  virtual String getKeyValue(String key, String filename = "/config.json")
  {

    StaticJsonBuffer<JSONCONFIGSIZE>  data;
		File f = SPIFFS.open(filename, "r");
		if (f) {
			size_t size = f.size();
			if ( size == 0 ) {
				f.close();
				return "";
			}
      else
      {
				std::unique_ptr<char[]> buf (new char[size]);
				f.readBytes(buf.get(), size);
				JsonObject& cfg = data.parseObject(buf.get());
				if (!cfg.success()) {
					return "";
				}
			f.close();
			return cfg[key];
			}
		}
    else
		{
			return "";
		}
  }

  virtual bool setKeyValue(String key,String value, String filename = "/config.json")
  {
		File f = SPIFFS.open(filename, "r");
		if ( (f) && (f.size() >0) )
		{
      StaticJsonBuffer<JSONCONFIGSIZE>  data;
			size_t size = f.size();
			std::unique_ptr<char[]> buf (new char[size]);
			f.readBytes(buf.get(), size);
			JsonObject& cfg = data.parseObject(buf.get());
			if (cfg.success()) {
				cfg[key] = value;
				f.close();
				yield();
				f = SPIFFS.open(filename, "w");
				cfg.printTo(f);
				f.close();
				yield();
				return true;
			}
			else
			{
				Serial.println(F("Bad JSON :("));
				return false;
			}

		}
		 else
		{
			StaticJsonBuffer<JSONCONFIGSIZE> data;
			JsonObject& root = data.createObject();
			root[key] = value;
			yield();
			f = SPIFFS.open(filename, "w");
			root.printTo(f);
			f.close();
			yield();
			return true;
		}
  }

  virtual String readFile(String file)
  {
    String result;
  	File f = SPIFFS.open(file, "r");
  	if (f) {
  		size_t size = f.size();
  		if ( size == 0 ) {
  			f.close();
  			Serial.println(F("Bad File!"));
  			return "";
  		}else{
        while(f.available())
        {
          result += (char)f.read();
        }
        f.close();
        return result;
  		}
  	}
    else
  	{
  		Serial.println(String(F("cannot open:")) + file);
  		return "";
  	}
  }

  virtual void writeFile(String file,String& data)
  {
    File f;
    Serial.println(String(F("writing: "))+file+String(F(" length:"))+String(data.length()));
    yield();
    f = SPIFFS.open(file, "w");

    if (f)
    {
      yield();
      for(uint i = 0 ; i < data.length(); i++)
      {
        f.write(data.charAt(i));
      }
      f.close();
      yield();
    }
    else
    {
      Serial.print(String(F("cannot write to "))+file);
      yield();
    }
  }

  virtual void initConfig()
  {
    Serial.println(String(F("Writing initial config!")));
    yield();
    StaticJsonBuffer<JSONCONFIGSIZE>  data;
    JsonObject& cfg = data.parseObject(stockConfig());
    if (!cfg.success())
    {
      Serial.println(String(F("cannot parse initial config!")));
      yield();
      return;
    }
    Serial.print(String(F("Stock config. size: "))+String(stockConfig().length())+" ,");
    Serial.print(cfg.size());
    Serial.println(String(F(" records found.")));
    yield();
    File f;
    f = SPIFFS.open(F("/config.json"), "w");
    if(f)
    {
      Serial.println(F("Opened file for write"));
      yield();
      cfg.printTo(f);
      Serial.println(F("Written"));
      f.close();
      yield();
    }
    else
    {
      Serial.println(F("cannot open file"));
    }

    Serial.println(F("checking new config..."));
    yield();
    if(hasStoredConfig())
      Serial.println(F("config OK!"));
    else
      Serial.println(F("ERROR!!!!! - Cannot save config:("));

  }

  virtual bool hasStoredConfig()
  {
    StaticJsonBuffer<JSONCONFIGSIZE>  data;
  	JsonObject& cfg = data.parseObject(readFile(F("/config.json")));
  	if (!cfg.success()) {
  		Serial.println(F("Parsing json failed !"));
  		return false;
  	}
    String version = cfg[CONFIG_VERSION];
    if(version.toInt() > 0)
    {
      Serial.println(String(F("Found settings, version :"))+version);
      StaticJsonBuffer<JSONCONFIGSIZE>  data2;
    	JsonObject& scfg = data2.parseObject(stockConfig());
      if (!scfg.success()) {
        Serial.println(F("ERROR!"));
      }
      else
      {
        String sver = scfg[CONFIG_VERSION];
        Serial.println(String(F("Stock version: "))+sver);
        if(sver.toInt() > version.toInt())
        {
          String(F(" updating..."));
          for (auto kvp : cfg) {
            scfg[kvp.key] = kvp.value;
          }
          scfg[CONFIG_VERSION] = sver;
          File f;
          f = SPIFFS.open(F("/config.json"), "w");
          if(f)
          {
            yield();
            scfg.printTo(f);
            f.close();
            yield();
          }
        }
      }

      return true;
    }
    else
    {
      Serial.println(F("no configVersion!"));
      return false;
    }
  }

  void dumpConfig()
  {
    StaticJsonBuffer<JSONCONFIGSIZE>  data;
  	JsonObject& cfg = data.parseObject(readFile(F("/config.json")));
  	if (!cfg.success()) {
  		return;
  	}
    cfg.prettyPrintTo(Serial);
  }

protected:

};

#endif //STORAGESS
