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

static void paintRainbow(std::vector<CRGB*>& array, animationCounters& ac, uint16_t steps)
{
  ac.c0 += steps;
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
