#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "qtcompat.h"

extern uint16_t timeSinceLastFrameMS;
extern uint16_t thisCycleMS;


struct bmp_file_header_t {
  uint16_t signature;
  uint32_t file_size;
  uint16_t reserved[2];
  uint32_t image_offset;
};

struct bmp_image_header_t {
  uint32_t header_size;
  uint32_t image_width;
  uint32_t image_height;
  uint16_t color_planes;
  uint16_t bits_per_pixel;
  uint32_t compression_method;
  uint32_t image_size;
  uint32_t horizontal_resolution;
  uint32_t vertical_resolution;
  uint32_t colors_in_palette;
  uint32_t important_colors;
};



static ledGadgetAnimations name2animation(String name)
{
    if     (name == "Fade")
        return animationFade;
    else if(name == "Glow")
        return animationGlow;
    else if(name == "Flash")
        return animationFlash;
    else if(name == "Strobe")
        return animationStrobe;
    else if(name == "FlashOnPeak")
        return animationFlashOnPeak;
    else if(name == "Sparks")
        return animationSparks;
    else if(name == "Cylon")
        return animationCylon;
    else if(name == "ChaoticLight")
        return animationChaoticLight;
    else if(name == "Rainbow")
        return animationRainbow;
    else if(name == "VUMeter")
        return animationVUMeter;
    else if(name == "VUMeterCentral")
        return animationVUMeterCentral;
    else if(name == "VUMeterHist")
        return animationVUMeterHist;
    else if(name == "DMX")
        return animationDMX;
    else if(name == "EQ")
        return animationEQ;
    else if(name == "EQ-Center")
        return animationEQCenter;
    else if(name == "EQ-Hist")
        return animationEQHist;
    else if(name == "ScrollText")
        return animationScrollText;
    else if(name == "ShowBPM")
        return animationShowBPM;
    else if(name ==  "Speed2Light")
        return animationSpeed2Light;

    return animationNone;
}

static String animation2name(ledGadgetAnimations a)
{
    if     (a == animationFade)
        return "Fade";
    else if(a == animationGlow)
        return "Glow";
    else if(a == animationFlash)
        return "Flash";
    else if(a == animationStrobe)
        return "Strobe";
    else if(a == animationFlashOnPeak)
        return "FlashOnPeak";
    else if(a == animationSparks)
        return "Sparks";
    else if(a == animationCylon)
        return "Cylon";
    else if(a == animationChaoticLight)
        return "ChaoticLight";
    else if(a == animationRainbow)
        return "Rainbow";
    else if(a == animationVUMeter)
        return "VUMeter";
    else if(a == animationVUMeterCentral)
        return "VUMeterCentral";
    else if(a == animationVUMeterHist)
        return "VUMeterHist";
    else if(a == animationDMX)
        return "DMX";
    else if(a == animationEQ)
        return "EQ";
    else if(a == animationEQCenter)
        return "EQ-Center";
    else if(a == animationEQHist)
        return "EQ-Hist";
    else if(a == animationScrollText)
        return "ScrollText";
    else if(a == animationShowBPM)
        return "ShowBPM";
    else if(a ==  animationSpeed2Light)
        return "Speed2Light";
    return "None";
}

static void copyLeds(std::vector<CRGB*>& src,std::vector<CRGB*>& dest)
{
    uint16_t i;
    for( i = 0 ; (i < src.size()) && (i < dest.size()) ; i++ )
        *dest.at(i) = *src.at(i);
    for( i ; i < dest.size() ; i++)
        *dest.at(i) = CRGB();
}

static std::vector<CRGB*> invertLedOrder(const std::vector<CRGB*>& array)
{
    std::vector<CRGB*> result;
    for(int i = array.size()-1 ; i>= 0  ; i-- )
    {
        result.push_back(array[i]);
    }
    return result;
}

static std::vector<std::vector<CRGB*> > getQuarters(std::vector<CRGB*>& src)
{
    std::vector<std::vector<CRGB*> > result;
    result.push_back(std::vector<CRGB*>());
    uint16_t size = src.size()/4;

    for(int i = 0 ; i < src.size() ; i++)
    {
        if(i > 0 && i%size == 0 )
            result.push_back(std::vector<CRGB*>());
        result.at(result.size()-1).push_back(src.at(i));
    }

    return result;
}

static std::vector<CRGB*> getRightHalf(std::vector<CRGB*>& src)
{
    std::vector<CRGB*> result;
    for(int i = src.size()-1 ; i > src.size()/2 ; i --)
    {
        result.push_back(src.at(i));
    }
    return result;
}

static std::vector<CRGB*> getLeftHalf(std::vector<CRGB*>& src)
{
    std::vector<CRGB*> result;
    for(int i = 0 ; i < src.size()/2 ; i ++)
    {
        result.push_back(src.at(i));
    }
    return result;
}

static void clearLedArray(std::vector<CRGB*>& array)
{
    for(uint16_t i = 0 ; i < array.size() ; i++)
        *array.at(i) = CRGB();
}

static CRGB calcVuColor(int level)
{
    if      (level <15){
        return CRGB(0,50,160);
    }else if(level <55){
        return CRGB(0,250,0);
    }else if(level <75){
        return CRGB(180,180,0);
    }else{
        return CRGB(250,0,0);
    }
}

static void paintVuMeterBar(std::vector<CRGB*>& array, uint8_t vuPercentage)
{
    if(vuPercentage>100) vuPercentage = 100;
    uint16_t lit = array.size()*(vuPercentage/100.0f);
    for(int i = 0 ; i < lit ; i++)
    {
        float f1 = i;
        float f2 = lit;
        float pos = f1/f2;
        int power = pos*vuPercentage;
        *array[i] = calcVuColor(power);
    }

    for(int i = lit ; i < array.size() ; i++ )
    {
        *array[i] = CRGB(0,0,0);
    }
}

static void paintVuMeterCentralBar(std::vector<CRGB*>& array, uint8_t vuPercentage)
{
    if(vuPercentage>100) vuPercentage = 100;
    int center   = array.size()/2;

    std::vector<CRGB*> upper;
    for(int i = center ; i < array.size() ; i++)
        upper.push_back(array.at(i));
    paintVuMeterBar(upper,vuPercentage);

    std::vector<CRGB*> bottom;
    for(int i = center-1 ; i >= 0 ; i--)
        bottom.push_back(array.at(i));
    paintVuMeterBar(bottom,vuPercentage);
}

static void paintEQ(std::vector<CRGB*>& array , uint16_t w, uint16_t h,uint8_t* spectrum,uint8_t spectrumSize = 7)
{
    clearLedArray(array);
    uint8_t barsize         = w/spectrumSize;
    uint8_t centralBarsize  = barsize + w%spectrumSize;
    uint8_t centralBarIndex = (spectrumSize/2);
    uint16_t col = 0;

    for(uint8_t band = 0 ; band < spectrumSize ; band++)
    {
        uint16_t colcount;
        if(band == centralBarIndex)
            colcount = centralBarsize;
        else
            colcount = barsize;

        for(uint16_t c = 0 ; c < colcount ; c++)
        {
            std::vector<CRGB*> lcol;
            for(uint16_t i = 0 ; i < h; i++)
            {
                uint16_t index = col + (i*w);
                lcol.push_back(array[index]);
            }
            lcol = invertLedOrder(lcol);
            paintVuMeterBar(lcol,spectrum[band]);
            col++;
        }
    }
}

static void paintEQCenter(std::vector<CRGB*>& array , uint16_t w, uint16_t h,uint8_t* spectrum,uint8_t spectrumSize = 7)
{
    clearLedArray(array);
    uint8_t barsize         = w/spectrumSize;
    uint8_t centralBarsize  = barsize + w%spectrumSize;
    uint8_t centralBarIndex = (spectrumSize/2);
    uint16_t col = 0;

    for(uint8_t band = 0 ; band < spectrumSize ; band++)
    {
        uint16_t colcount;
        if(band == centralBarIndex)
            colcount = centralBarsize;
        else
            colcount = barsize;

        for(uint16_t c = 0 ; c < colcount ; c++)
        {
            std::vector<CRGB*> lcol;
            for(uint16_t i = 0 ; i < h; i++)
            {
                uint16_t index = col + (i*w);
                lcol.push_back(array[index]);
            }
            lcol = invertLedOrder(lcol);
            paintVuMeterCentralBar(lcol,spectrum[band]);
            col++;
        }
    }
}

static void paintEQHist(std::vector<CRGB*>& array , uint16_t w, uint16_t h,std::vector<std::vector<uint8_t> >& spectrumHist,uint8_t spectrumSize = 7)
{
    clearLedArray(array);
    uint8_t barsize         = w/spectrumSize;
    uint8_t centralBarsize  = barsize + w%spectrumSize;
    uint8_t centralBarIndex = (spectrumSize/2);
    uint16_t col = 0;

    for(uint8_t band = 0 ; band < spectrumSize ; band++)
    {
        uint16_t colcount;
        if(band == centralBarIndex)
            colcount = centralBarsize;
        else
            colcount = barsize;


        for(int r = 0 ; r < h ; r++)
        {
            std::vector<CRGB*> row;
            for(uint16_t c = 0 ; c < colcount ; c++)
            {
                uint16_t index = col+c+(r*w);
                row.push_back(array[index]);
            }
            paintVuMeterCentralBar(row,spectrumHist[band][spectrumHist[band].size()-1-r]);
        }
        col+= colcount;
    }
}

static void paintCylon(std::vector<CRGB*>& array, uint16_t& c0,uint16_t& c1,uint16_t& c2)
{

    c0 += timeSinceLastFrameMS;
    if(c0 > 25)
        c0 = 0;
    else
        return;

    uint16_t min = 0;
    uint16_t max = array.size();
    uint16_t count = max - min;

    if(c2 == 0)//incrementando
    {
        int i = 0;
        for(int l = min ; l < max-1 ; l++ )
        {
            if(i == c1)
                *array[i] = CRGB(255,0,0);
             else
                array[i]->r *= 0.85f;
                array[i]->g *= 0.85f;
                array[i]->b *= 0.85f;
            if(c1 >= max-1)
            {
                c2 = 1;
                return;
            }
            else
                i++;
        }
        c1++;
    }
    else //decrementando
    {
        uint16_t i = count-1;
        for(int l = max-1 ; l >= min ; l-- )
        {
            if(i == c1)
               *array[i] = CRGB(255,0,0);
            else
               array[i]->r *= 0.85f;
               array[i]->g *= 0.85f;
               array[i]->b *= 0.85f;
            if(c1 <= min)
            {
                c1 = 0;
                c2 = 0;
                return;
            }
            else
                i--;
        }
        c1--;
    }
}

static void paintRainbow(std::vector<CRGB*>& array, uint16_t& c0)
{
    if(c0 > 256*5) c0 = 0;

    for(uint16_t i = 0;  i < array.size() ; i++)
    {
        CRGB* c = array[i];
        uint16_t WheelPos = 255 - (((i * 256 / array.size()) + c0) & 255);
        if(WheelPos < 85)
        {
            *c = CRGB(255 - WheelPos * 3, 0, WheelPos * 3);
        }
        else if(WheelPos < 170)
        {
            WheelPos -= 85;
            *c = CRGB(0, WheelPos * 3, 255 - WheelPos * 3);
        }
        else
        {
            WheelPos -= 170;
            *c = CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
        }
    }

    if(timeSinceLastFrameMS>4)
        c0 += (timeSinceLastFrameMS)/3;
    else
        c0 += 1;
}

static void paintSparks(std::vector<CRGB*>& array)
{
    uint16_t index0 = rand() % array.size();
    uint16_t index1 = rand() % array.size();
    uint16_t index2 = rand() % array.size();
    uint16_t index3 = rand() % array.size();

    for(uint i = 0 ; i < array.size() ; i++)
    {
        if( (i == index0) ||
            (i == index1) ||
            (i == index2) ||
            (i == index3) )

            *array[i] = CRGB(240,240,240);
        else
        {
            array[i]->r *= (rand()%80+40)/100.0f;
            array[i]->g *= (rand()%80+40)/100.0f;
            array[i]->b *= (rand()%80+40)/100.0f;
        }
    }
}
#endif // ANIMATIONS_H


