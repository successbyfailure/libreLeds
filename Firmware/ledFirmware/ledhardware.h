#ifndef LEDHARDWARE
#define LEDHARDWARE
#include "qtcompat.h"

#include "settings.h"

class ledHardWare{
public:
    class led{
    public:
      led(CRGB* color)
      {
        m_color = color;
      }

      virtual void setColor(CRGB ncolor)
      {
          *m_color = ncolor;
      }

      virtual void setColor(uint8_t r, uint8_t g, uint8_t b)
      {
          m_color->r = r;
          m_color->g = g;
          m_color->b = b;
      }

      virtual void dimm(uint8_t power)
      {
        m_color->fadeToBlackBy(power);
      }

      virtual void dimm(float factor = 0.25)
      {
        uint8_t p = factor * 255;
        dimm(p);
      }

      bool isOn()
      {
          CRGB c = getColor();
          return (c.r > 0) || (c.b > 0) || (c.g > 0);
      }

      void off()                {setColor(0,0,0);}
      void white()              {setColor(200,200,200);}
      void red  (int r = 200)   {setColor(r,0,0);}
      void green(int g = 200)   {setColor(0,g,0);}
      void blue (int b = 200)   {setColor(0,0,b);}


      CRGB    getColor(){return *m_color;}
      uint8_t r()       {return m_color->r;}
      uint8_t g()       {return m_color->g;}
      uint8_t b()       {return m_color->b;}

    protected:
        CRGB* m_color;
    };

    ledHardWare(extraSettings* es) : m_settings(es) , m_ledCount(es->ledCount)
    {

    }

    virtual void     setup()   {;}
    virtual void     refresh() {;}

    void             setBrightnessF(float f)
    {
        uint8_t p = f*255;
        Serial.print("BF: ");Serial.print(f);Serial.print(" v:");Serial.println(p);
        setBrightness(p);
    }

    virtual void     setBrightness(uint8_t b) {;}

    virtual uint16_t ledCount()               {return m_ledCount;}

    virtual uint16_t width()                  {return ledCount();}

    virtual uint16_t height()                 {return 1;}

    virtual uint16_t off()
    {
        for(int i = 0 ; i < m_ledArray.size() ; i++)
            *m_ledArray[i] = CRGB(0,0,0);
    }

    virtual void  setLedColor(uint16_t index, CRGB color)
    {
        m_leds[index] = color;
    }

    virtual void  setLedColor(uint16_t index, uint8_t r,uint8_t g,uint8_t b)
    {
        CRGB c;
        c.r = r;
        c.g = g;
        c.b = b;
        m_leds[index] = c;
    }

    virtual CRGB&  getLedColor(uint16_t index)
    {
        return m_leds[index];
    }

    led     getLed(uint16_t index)          {return led(&m_leds[index]);}
    std::vector<CRGB*> getColorArray()      {return m_ledArray;}
    bool    isMatrix()                      {return m_isMatrix;}

    virtual void test()
    {
      for( uint16_t i = 0 ; i < ledCount() ; i++)
      {
        m_leds[i] = CRGB(255,255,255);
        if(i>=5)
            m_leds[i-5] = CRGB(0,0,0);
        if(i%2 == 0)refresh();
        if(i%25 == 0)yield();
      }
      off();
      refresh();
    }

    virtual void initAnimation()
    {
        uint16_t index = rand() % m_ledArray.size();
        for(uint i = 0 ; i < m_ledArray.size() ; i++)
        {
            if(i == index)
                getLed(i).white();
            else
                getLed(i).dimm();
        }
    }


protected:
    bool                m_isMatrix = false;
    extraSettings*      m_settings;
    CRGB*               m_leds;
    uint16_t            m_ledCount;
    std::vector<CRGB*>  m_ledArray;

};

class ws2812Strip : public ledHardWare{
public:
    ws2812Strip(extraSettings* es) : ledHardWare(es),
        m_strip(es->ledCount, es->ledPin, NEO_RGB + NEO_KHZ800)
    {
        m_leds = (CRGB*) m_strip.getDataArray();
        for(uint16_t i = 0 ; i < es->ledCount ; i++)
        {
            m_ledArray.push_back(&m_leds[i]);
        }
    }
    virtual void     setBrightness(uint8_t b) {m_strip.setBrightness(b);Serial.print("Bright(i):");Serial.println(b);}
    virtual void     setup()
    {
        m_strip.begin();
        setBrightnessF(m_settings->ledMaxBright);
    }

    virtual void     refresh()
    {
        m_strip.show();
    }

protected:
    Adafruit_NeoPixel m_strip;

};

class ledMatrixHW : public ledHardWare
{
public:
    ledMatrixHW(extraSettings* es) : ledHardWare(es)
    {

    }

    virtual void setPixel(uint16_t x, uint16_t y,CRGB color)
    {

    }

    virtual void initZigZagArray()
    {
        for(uint16_t i = 0 ; i < height() ; i++)
        {
            std::vector<CRGB*> row = getRow(i);
            for(int i = 0 ; i < row.size() ; i++)
            {
                m_ledArray.push_back(row[i]);
            }
        }
    }

    virtual std::vector<CRGB*> getRow(uint16_t r)
    {
        std::vector<CRGB*> result;

        uint16_t ledCount  = m_settings->ledCount;
        uint16_t xTileSize = m_settings->ledMatrixXSize;
        uint16_t yTileSize = m_settings->ledMatrixYSize;
        uint16_t xTiles    = m_settings->ledMatrixXTiles;
        uint16_t yTiles    = m_settings->ledMatrixYTiles;

        for(uint16_t tX = 0 ; tX < xTiles ; tX++)
        {
            for(uint16_t x = 0 ; x < xTileSize ; x++)
            {
                uint16_t tY = r / yTileSize;
                uint16_t tileOffset = (tY*yTileSize*xTileSize*xTiles) + (yTileSize*xTileSize*tX);
                uint16_t index = tileOffset;

                uint16_t y = 0;

                if(tY)
                    y = r%yTileSize;
                else
                    y = r;

                if(x%2)
                    index += ((x+1)*yTileSize)-y-1;
                else
                    index += (x*yTileSize)+y;

                if(index < ledCount)
                    result.push_back(&m_leds[index]);
            }
        }
        return result;
    }

    virtual std::vector<CRGB*> getCol(uint16_t c)
    {
        std::vector<CRGB*> result;

        uint16_t ledCount  = m_settings->ledCount;
        uint16_t xTileSize = m_settings->ledMatrixXSize;
        uint16_t yTileSize = m_settings->ledMatrixYSize;
        uint16_t xTiles    = m_settings->ledMatrixXTiles;
        uint16_t yTiles    = m_settings->ledMatrixYTiles;

        for(uint16_t tY = 0 ; tY < yTiles ; tY++)
        {
            for(uint16_t y = 0 ; y < yTileSize ; y++)
            {
                uint16_t tX = c / xTileSize;
                uint16_t tileOffset = (tY*yTileSize*xTileSize*xTiles) + (yTileSize*xTileSize*tX);
                uint16_t index = tileOffset;

                uint16_t x = 0;

                if(tX)
                    x = c%xTileSize;
                else
                    x = c;

                index += (x*yTileSize);
                if(x%2)
                    index += y;
                else
                    index += yTileSize - y-1;

                if(index < ledCount)
                    result.push_back(&m_leds[index]);
            }
        }
        return result;
    }

    virtual void drawText(String str,uint16_t x,uint16_t y,CRGB textColor = CRGB(255,0,0), CRGB background = CRGB(0,0,0)) {;}

    virtual uint16_t width()
    {
        return m_settings->ledMatrixXSize * m_settings->ledMatrixXTiles;
    }

    virtual uint16_t height()
    {
        return m_settings->ledMatrixYSize * m_settings->ledMatrixYTiles;
    }

    virtual void test()
    {
      for( uint16_t i = 0 ; i < width() ; i++)
      {
        std::vector<CRGB*> col = getCol(i);
        for(uint16_t l = 0 ; l < col.size() ; l++)
            *col[l] = CRGB(255,255,255);

        if(i>=3)
        {
            std::vector<CRGB*> col = getCol(i-3);
            for(uint16_t l = 0 ; l < col.size() ; l++)
                *col[l] = CRGB(0,0,0);
        }

        refresh();
        yield();
      }
      off();
      refresh();
    }

protected:

};

class ws2812Matrix : public ledMatrixHW{
public:
    ws2812Matrix(extraSettings* es) : ledMatrixHW(es),
        m_matrix(es->ledMatrixXSize, es->ledMatrixYSize,es->ledMatrixXTiles,es->ledMatrixYTiles, es->ledPin,
         NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_ROWS   + NEO_TILE_PROGRESSIVE +
         NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
         NEO_RGB + NEO_KHZ800)
    {
        m_leds = (CRGB*) m_matrix.getDataArray();
        initZigZagArray();
    }

    virtual void     setBrightness(uint8_t b) {m_matrix.setBrightness(b);}

    virtual void     setup()
    {
        m_matrix.begin();
        setBrightnessF(m_settings->ledMaxBright);
    }

    virtual void     refresh()
    {
        m_matrix.show();
    }

    virtual void drawText(String str,uint16_t x,uint16_t y,CRGB textColor = CRGB(255,0,0), CRGB background = CRGB(0,0,0))
    {
        m_matrix.fillScreen(m_matrix.Color(background.r,background.g,background.b));
        m_matrix.setTextColor(m_matrix.Color(textColor.r,textColor.g,textColor.b));
        m_matrix.setCursor(x, y);
        m_matrix.print(str);
    }

protected:
    Adafruit_NeoMatrix m_matrix;
};

class apa102Strip : public ledHardWare
{
public:
    apa102Strip(extraSettings* es) : ledHardWare(es)
    {
        m_leds = new CRGB[m_ledCount];
        for(uint16_t i = 0 ; i < es->ledCount ; i++)
        {
            m_ledArray.push_back(&m_leds[i]);
        }
    }

    virtual void     refresh()                {FastLED.show();}
    virtual void     setBrightness(uint8_t b) {LEDS.setBrightness(b);}
    virtual void     setup()
    {
        LEDS.addLeds<APA102, LED_PIN,LED_CLOCK,BGR>(m_leds, m_ledCount);
        setBrightnessF(m_settings->ledMaxBright);
    }
};

class apa102Matrix : public ledMatrixHW
{
public:
    apa102Matrix(extraSettings* es) : ledMatrixHW(es)
    {
        m_leds = new CRGB[m_ledCount];
        initZigZagArray();
    }

    virtual void     refresh()                {FastLED.show();}
    virtual void     setBrightness(uint8_t b) {LEDS.setBrightness(b);}
    virtual void     setup()
    {
        LEDS.addLeds<APA102, LED_PIN,LED_CLOCK,BGR>(m_leds, m_ledCount);
        setBrightnessF(m_settings->ledMaxBright);
    }
};

#endif // LEDHARDWARE
