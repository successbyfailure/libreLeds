#ifndef EEPROMSTORAGE
#define EEPROMSTORAGE


#include "settings.h"
#include "protocol.h"
#include "qtcompat.h"

class settingsStorage
{
public:
    settingsStorage()
    {
        Serial.begin(115200);
        Serial.print("settings storage init ...");
    }

   ~settingsStorage() {;}

    void save()
    {
        saveBasicSettings();
        yield();
        saveExtraSettings();
    }

    void load()
    {
        Serial.print("\tBasic Settings:");
        if(loadBasicSettings())
            Serial.print("Loaded!");
        else
            Serial.print("Hardcoded");
        Serial.print("\tExtra Settings:");
        if(loadExtraSettings())
            Serial.print("Loaded!");
        else
            Serial.print("Hardcoded");
        Serial.print("\n");
    }

    basicSettings&  getBasicSettings()
    {
        return m_basicSettings;
    }

    extraSettings&  getExtraSettings()
    {
        return m_extraSettings;
    }

    void getBasicSettingsBytes(uint8_t* result)
    {
        copyRawBytes((uint8_t*)&m_basicSettings,result,sizeof(basicSettings));
    }

    void getExtraSettingsBytes(uint8_t* result)
    {
        copyRawBytes((uint8_t*)&m_extraSettings,result,sizeof(extraSettings));
    }

    void setBasicSettingsBytes(uint8_t* byteStorage)
    {
        basicSettings result;
        result.magicNumber = 0;
        copyRawBytes(byteStorage,(uint8_t*)&result,sizeof(basicSettings));
        if(result.magicNumber == basicSettings().magicNumber)
            m_basicSettings = result;
    }

    void setExtraSettingsBytes(uint8_t* byteStorage)
    {
        extraSettings result;
        result.magicNumber = 0;
        copyRawBytes(byteStorage,(uint8_t*)&result,sizeof(extraSettings));
        if(result.magicNumber == extraSettings().magicNumber)
            m_extraSettings = result;
    }

    void setBasicSettingsBytes(String& str)
    {
        basicSettings result;
        result.magicNumber = 0;
        copyRawBytes(str,(uint8_t*)&result,sizeof(basicSettings));
        if(result.magicNumber == basicSettings().magicNumber)
        {
            Serial.print("New Settings! id:");Serial.println(result.id);
            m_basicSettings = result;

        }
        else
            Serial.println("Bad Settings!");
    }

    void setExtraSettingsBytes(String& str)
    {
        extraSettings result;
        result.magicNumber = 0;
        copyRawBytes(str,(uint8_t*)&result,sizeof(extraSettings));
        if(result.magicNumber == extraSettings().magicNumber)
        {
            Serial.println("New Settings!");
            m_extraSettings = result;
        }
        else
            Serial.println("Bad Settings!");
    }



    virtual bool            hasBasicSettings(){;}
    virtual bool            hasExtraSettings(){;}
    virtual void            clear(){;}

protected:
    basicSettings   m_basicSettings;
    extraSettings   m_extraSettings;

    virtual bool loadBasicSettings() {;}
    virtual bool loadExtraSettings() {;}
    virtual void saveBasicSettings() {;}
    virtual void saveExtraSettings() {;}
};


class EEPROMStorage : public settingsStorage
{
public:
  EEPROMStorage() : settingsStorage()
  {
      load();
      Serial.println("EEPROM ready!");
  }

 ~EEPROMStorage() {;}

  bool hasBasicSettings()
  {
      basicSettings settings;
      settings.magicNumber = 0;
      EEPROM.begin(m_flashSize);
      EEPROM.get(0,settings);
      EEPROM.end();
      return settings.magicNumber == basicSettings().magicNumber;
  }

  bool hasExtraSettings()
  {
      extraSettings settings;
      settings.magicNumber = 0;
      EEPROM.begin(m_flashSize);
      EEPROM.get(sizeof(basicSettings),settings);
      EEPROM.end();
      return settings.magicNumber == extraSettings().magicNumber;
  }

  void clear()
  {
      EEPROM.begin(m_flashSize);
      for ( int i = 0 ; i < m_flashSize ; i++ )
        EEPROM.write(i, 0);
      EEPROM.end();
  }


protected:
  uint16_t    m_flashSize   = 1024;

  bool loadBasicSettings()
  {
      basicSettings settings;
      settings.magicNumber = 0;
      EEPROM.begin(m_flashSize);
      EEPROM.get(0,settings);
      EEPROM.end();

      if(settings.magicNumber == basicSettings().magicNumber)
      {
          m_basicSettings = settings;
          return true;
      }
      else
          return false;
  }

  bool loadExtraSettings()
  {
      extraSettings settings;
      settings.magicNumber = 0;
      EEPROM.begin(m_flashSize);
      EEPROM.get(sizeof(basicSettings),settings);
      EEPROM.end();

      if(settings.magicNumber == extraSettings().magicNumber)
      {
          m_extraSettings = settings;
          return true;
      }
      else
          return false;
  }

  void saveBasicSettings()
  {
      EEPROM.begin(m_flashSize);
      Serial.print("Saving Basic Settings..");
      EEPROM.put(0,m_basicSettings);
      EEPROM.commit();
      EEPROM.end();
  }

  void saveExtraSettings()
  {
      EEPROM.begin(m_flashSize);
      Serial.print("Saving Extra Settings..");
      EEPROM.put(sizeof(basicSettings),m_extraSettings);
      EEPROM.commit();
      EEPROM.end();
  }
};
#endif // EEPROMSTORAGE

