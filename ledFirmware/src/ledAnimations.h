#ifndef ANIMATIONS
#define ANIMATIONS


//
// Array tools
//

static void copyLeds(std::vector<CRGB*>& src,std::vector<CRGB*>& dest)
{
    uint16_t i;
    for( i = 0 ; (i < src.size()) && (i < dest.size()) ; i++ )
        *dest.at(i) = *src.at(i);
    for( uint16_t c = i ; c < dest.size() ; c++)
        *dest.at(c) = CRGB();
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

    for(uint i = 0 ; i < src.size() ; i++)
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
    for(uint i = src.size()-1 ; i > src.size()/2 ; i --)
    {
        result.push_back(src.at(i));
    }
    return result;
}

static std::vector<CRGB*> getLeftHalf(std::vector<CRGB*>& src)
{
    std::vector<CRGB*> result;
    for(uint i = 0 ; i < src.size()/2 ; i ++)
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


static int random(int min, int max)
{
   return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}
//
// Animations
//

struct animationCounters
{
    uint16_t c0 = 0;
    uint16_t c1 = 0;
    uint16_t c2 = 0;
    uint16_t c3 = 0;
    uint16_t c4 = 0;
    uint16_t c5 = 0;
};

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

    for(uint i = lit ; i < array.size() ; i++ )
    {
        *array[i] = CRGB(0,0,0);
    }
}

static void paintVuMeterCentralBar(std::vector<CRGB*>& array, uint8_t vuPercentage)
{
    if(vuPercentage>100) vuPercentage = 100;
    int center   = array.size()/2;

    std::vector<CRGB*> upper;
    for(uint i = center ; i < array.size() ; i++)
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

static void paintChaoticLight(std::vector<CRGB*>& array)
{
    for(uint16_t i = 0 ; i < array.size() ; i++)
    {
        uint8_t random = rand() % 3;
        if(random == 0)
            *array[i] = CRGB(250,0,0);
        else if(random == 1)
            *array[i] = CRGB(0,250,0);
        else if(random == 2)
            *array[i] = CRGB(0,0,250);
    }
}

static void paintRainbow(std::vector<CRGB*>& array, animationCounters& ac, uint16_t steps)
{
  ac.c0 += steps/5;
  if(ac.c0 > 256*5) ac.c0 = 0;
  for(uint16_t i = 0;  i < array.size() ; i++)
  {
      CRGB* c = array[i];
      uint16_t WheelPos = 255 - (((i * 256 / array.size()) + ac.c0) & 255);
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

}

static void paintSparks(std::vector<CRGB*>& array, uint16_t steps)
{
  for(uint i = 0 ; i < array.size() ; i++)
  {
    array[i]->r *= random(95-(steps/2),100) /100.0f;
    array[i]->g *= random(95-(steps/2),100) /100.0f;
    array[i]->b *= random(95-(steps/2),100) /100.0f;
  }

  uint8_t newDots = ((array.size()*0.01) * (steps/3.0))/5;
  if( (newDots < 1) && random(0,1) )
    newDots = 1;

  for(int i = 0 ; i < newDots ; i++)
  {
    uint16_t index = rand() % array.size();
    *array[index]  = CRGB(200,200,200);
  }
}

static void paintCylon(std::vector<CRGB*>& array, animationCounters& ac, uint16_t steps)
{

    ac.c0 += steps;
    if(ac.c0 > 25)
        ac.c0 = 0;
    else
        return;

    uint16_t min = 0;
    uint16_t max = array.size();
    uint16_t count = max - min;

    if(ac.c2 == 0)//incrementando
    {
        int i = 0;
        for(int l = min ; l < max-1 ; l++ )
        {
            if(i == ac.c1)
                *array[i] = CRGB(255,0,0);
             else
                array[i]->r *= 0.85f;
                array[i]->g *= 0.85f;
                array[i]->b *= 0.85f;
            if(ac.c1 >= max-1)
            {
                ac.c2 = 1;
                return;
            }
            else
                i++;
        }
        ac.c1++;
    }
    else //decrementando
    {
        uint16_t i = count-1;
        for(int l = max-1 ; l >= min ; l-- )
        {
            if(i == ac.c1)
               *array[i] = CRGB(255,0,0);
            else
               array[i]->r *= 0.85f;
               array[i]->g *= 0.85f;
               array[i]->b *= 0.85f;
            if(ac.c1 <= min)
            {
                ac.c1 = 0;
                ac.c2 = 0;
                return;
            }
            else
                i--;
        }
        ac.c1--;
    }
}

#endif
