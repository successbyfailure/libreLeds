#ifndef SETTINGS
#define SETTINGS

#include "qtcompat.h"
#include "protocol.h"

#define MAXSTRLENGTH 20
#define ML MAXSTRLENGTH


enum ledHardwareType
{
    hwWS2812Strip,
    hwWS2812Matrix,
    hwWS2801Strip,
    hwAPA102Strip,
    hwAPA102Matrix
};

enum ledGadgetType
{
    gadgetNone,
    gadgetDMX,
    gadgetSpotLed,
    gadgetLedBar,
    gadgetLedCircle,
    gadgetLedWheel,
    gadgetledMatrix,
    gadgetledTree
};

enum wMode
{
    wifiAP,
    wifiClient,
    wifiMesh,
    wifiOverride
};

struct basicSettings
{
    char id[20]                     = "LedGadget";

    //Watchdog
    uint16_t watchdogTime           = 2500;
    uint16_t delayTime              = 5;
    uint16_t highFreqCycleMs        = 1;
    uint16_t medFreqCycleMs         = 30;
    uint16_t lowFreqCycleMs         = 250;


    //Wireless
    wMode    wifiMode               =  wifiAP;
    char     wifiESSID         [ML] = "LedGadget";
    char     wifiPasswd        [ML] = "clubmate";
    char     remoteHost        [ML] = "ledmaster.local";
    uint16_t remotePort             =  31416;

    char     wifiOverrideESSID [ML] = "AndroidAP"; //Si este ssid esta presente en el arranque se conectara con el.
    char     wifiOverridePasswd[ML] = "clubmate";
    char     overrideRemoteHost[ML] = "192.168.43.1";
    uint16_t overrideRemotePort     =  31416;

    uint16_t localPort              = 31416;
    uint16_t httpUpdatePort         = 8080;
    uint16_t httpPort               = 80;
    uint8_t  IP[4]                  = {2,44,97,150};
    uint8_t  subnet[4]              = {255,255,0,0};

    bool     staticIP                  = false;
    bool     allowWifiOverride         = true;
    bool     autoConnectRemote         = false;
    bool     overrideAutoConnectRemote = true;
    bool     localPortEnabled          = true;
    bool     httpUpdaterEnabled        = true;
    bool     httpServerEnabled         = true;
    bool     MdnsEnabled               = true;
    bool     serialClient              = false;

    uint16_t magicNumber               = 31415; //numero de comprobacion para saber que los settings se han leido/transmitido bien.
};

struct extraSettings
{
    //Pinconfig
    int8_t  alivePin           = D0;
    //ledPins
    int8_t  ledPin             = D4;
    int8_t  ledClock           = D3;

    //i2c
    int8_t sdaPin              = D2;
    int8_t sclPin              = D1;

    //MSGEQ7
    int8_t eqOut               = A0;
    int8_t eqReset             = D5;
    int8_t eqStrobe            = D6;

    //Buttons
    int8_t btn1Pin             = D7;
    int8_t btn2Pin             = D8;

    //LedConfig
    ledHardwareType ledhwType   = hwWS2812Strip;
    ledGadgetType   gadgetType  = gadgetLedBar;
    ledGadgetAnimations
               defaultAnimation = animationCylon;
    uint16_t ledCount           = 84;
    float    ledMaxBright       = 0.15;
    bool     ledReversedOrder   = false;
    bool     ledMatrixZigZag    = true;
    uint16_t ledMatrixXSize     = 16;
    uint16_t ledMatrixYSize     = 16;
    uint16_t ledMatrixXTiles    = 3;
    uint16_t ledMatrixYTiles    = 1;

    //Watchdog
    uint16_t watchdogTime       = 2500;

    //ArtNet
    bool     artNetEnabled      = true;
    uint16_t startUniverse      = 1;
    uint16_t magicNumber        = 31415;//numero de comprobacion para saber que los settings se han leido/transmitido bien.
};

#endif // SETTINGS

