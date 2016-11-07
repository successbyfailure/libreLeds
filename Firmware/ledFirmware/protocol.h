#ifndef PROTOCOL
#define PROTOCOL

#include "vector"
#include "qtcompat.h"


#define P_PACKETSTART "[[[[|||"
#define P_PACKETEND   "|||]]]]"


#define MAX_SRV_CLIENTS 3

#define DATAPACKET_MAXFIELDNAME 10
#define DATAPACKET_FIELDNR      8
#define ANIMATIONLIST_MAXITEMS  10
#define ANIMATIONLIST_MAXSTR    21



enum ledGadgetAnimations
{
    animationNone,
    animationDMX,
    animationFade,
    animationGlow,
    animationFlash,
    animationStrobe,
    animationFlashOnPeak,
    animationSparks,
    animationCylon,
    animationChaoticLight,
    animationRainbow,
    animationVUMeter,
    animationVUMeterCentral,
    animationVUMeterHist,
    animationEQ,
    animationEQCenter,
    animationEQHist,
    animationScrollText,
    animationShowBPM
};



enum basicProtocolMessages
{
    cmdDummy,
    cmdGetSettings,
    cmdReloadSettings,
    cmdSaveSettings,
    cmdFactorySettings,
    cmdReset,
    cmdPing,
    respPong
};

struct simpleProtocolMessage
{
    uint16_t    msgID       = cmdDummy;
    uint16_t    magicNumber = 31416;
};

struct dataReadingsPacket
{
    char    source[10]  = "none";

    uint8_t fieldNameMaxLength                  = DATAPACKET_MAXFIELDNAME;
    uint8_t fieldNr                             = DATAPACKET_FIELDNR;

    char    fields[DATAPACKET_FIELDNR][DATAPACKET_MAXFIELDNAME] = {""};
    float   values[DATAPACKET_FIELDNR]          = {-1};

    uint16_t magicNumber                        = 31416;
};

enum ledGadgetProtocol
{
    cmdNone,
    cmdGetAnimationList,
    cmdSetAnimation,
    rspCurrentAnimation,
    cmdOff,
    cmdSetColor,
    cmdSetBrigth,
    cmdDimm,
    msgPeak,
    msgVuLevel
};


struct ledGadgetProtocolPacket
{
    ledGadgetProtocol cmd            = cmdNone;
    uint8_t           intVal0        = 0;
    uint8_t           intVal1        = 0;
    uint8_t           intVal2        = 0;
    uint8_t           intVal3        = 0;
    uint8_t           intVal4        = 0;
    uint8_t           intVal5        = 0;
    uint8_t           intVal6        = 0;
    uint8_t           intVal7        = 0;

    float             floatVal0      = 0;
    float             floatVal1      = 0;
    float             floatVal2      = 0;
    float             floatVal3      = 0;

    uint16_t          magicNumer     = 616;
};

struct ledGadgetAnimationListPacket
{
    uint8_t              usedEntries                         = 0;
    ledGadgetAnimations  animationID[ANIMATIONLIST_MAXITEMS] = {animationNone};
    char                 animationName[ANIMATIONLIST_MAXITEMS][ANIMATIONLIST_MAXSTR] = {{NULL}};
    uint16_t             magicNumer                          = 626;
};



struct imgMessage
{

};

static void copyRawBytes(uint8_t* source, uint8_t* dest,uint16_t size)
{
    for(uint16_t i = 0 ; i < size ; i++)
        dest[i] = source[i];
}

#ifdef ESP8266
static void copyRawBytes(String& source, uint8_t* dest,uint16_t size)
{
    for(uint16_t i = 0 ; i < size ; i++)
        dest[i] = (uint8_t)source.charAt(i);
}

static void copyRawBytes(String& source, simpleProtocolMessage& dest)
{
    uint8_t* byteStorage = (uint8_t*)&dest;
    for(uint16_t i = 0 ; i < sizeof(simpleProtocolMessage) ; i++)
        byteStorage[i] = (uint8_t)source.charAt(i);
}
#endif
#endif // PROTOCOL

