#include "editextrasettingsgroupbox.h"
#include "ui_editextrasettingsgroupbox.h"

editExtraSettingsGroupBox::editExtraSettingsGroupBox(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::editExtraSettingsGroupBox)
{
    ui->setupUi(this);
    populateComboBoxes();
    ui->gbMatrix->hide();
    connect(ui->spinMatrixTX,SIGNAL(valueChanged(int)),this,SLOT(updateLedCount()));
    connect(ui->spinMatrixTY,SIGNAL(valueChanged(int)),this,SLOT(updateLedCount()));
    connect(ui->spinMatrixY ,SIGNAL(valueChanged(int)),this,SLOT(updateLedCount()));
    connect(ui->spinMatrixX ,SIGNAL(valueChanged(int)),this,SLOT(updateLedCount()));

    connect(ui->comboLedHWType ,SIGNAL(currentIndexChanged(int)),this,SLOT(enableMatrix(int)));
}

editExtraSettingsGroupBox::~editExtraSettingsGroupBox()
{
    delete ui;
}

void editExtraSettingsGroupBox::setClient(protocolClient* c)
{
    m_client =(ledController*) c;
    connect(c   ,SIGNAL(extraSettingsReceived(extraSettings)),
            this,SLOT  (loadSettings(extraSettings)));

    loadSettings();
    connect(c,SIGNAL(connected(bool))   ,this,SLOT(setEnabled(bool)));
    connect(c,SIGNAL(disconnected(bool)),this,SLOT(setDisabled(bool)));
    connect(ui->btnSave,SIGNAL(clicked(bool)),c,SLOT(saveSettings()));
    connect(ui->btnSend,SIGNAL(clicked(bool)),this,SLOT(sendSettings()));
}

void editExtraSettingsGroupBox::loadSettings()
{
    loadSettings(m_client->getExtraSettings());
}

void editExtraSettingsGroupBox::loadSettings(extraSettings s)
{

    ui->comboAlivePin->setCurrentIndex      (getComboBoxItemIndex(ui->comboAlivePin,GPIO2Pin(s.alivePin)));
    ui->comboLedPin->setCurrentIndex        (getComboBoxItemIndex(ui->comboLedPin,GPIO2Pin(s.ledPin)));
    ui->comboLedClock->setCurrentIndex      (getComboBoxItemIndex(ui->comboLedClock,GPIO2Pin(s.ledClock)));
    ui->comboSDA->setCurrentIndex           (getComboBoxItemIndex(ui->comboSDA,GPIO2Pin(s.sdaPin)));
    ui->comboSCL->setCurrentIndex           (getComboBoxItemIndex(ui->comboSCL,GPIO2Pin(s.sclPin)));
    ui->comboEQOut->setCurrentIndex         (getComboBoxItemIndex(ui->comboEQOut,GPIO2Pin(s.eqOut)));
    ui->comboEQReset->setCurrentIndex       (getComboBoxItemIndex(ui->comboEQReset,GPIO2Pin(s.eqReset)));
    ui->comboEQStrobe->setCurrentIndex      (getComboBoxItemIndex(ui->comboEQStrobe,GPIO2Pin(s.eqStrobe)));
    ui->comboButton1Pin->setCurrentIndex    (getComboBoxItemIndex(ui->comboButton1Pin,GPIO2Pin(s.btn1Pin)));
    ui->comboButton2Pin->setCurrentIndex    (getComboBoxItemIndex(ui->comboButton2Pin,GPIO2Pin(s.btn2Pin)));

    ui->comboLedHWType->setCurrentIndex     (s.ledhwType);

    if(s.ledhwType == 1)
        ui->gbMatrix->show();
    else
        ui->gbMatrix->hide();

    ui->comboLedGadgetType->setCurrentIndex    (s.gadgetType);
    ui->comboDefaultAnimation->setCurrentIndex (s.defaultAnimation);
    ui->spinR->setValue                        (s.defaultColor[0]);
    ui->spinG->setValue                        (s.defaultColor[1]);
    ui->spinB->setValue                        (s.defaultColor[2]);
    ui->spinLedCount->setValue                 (s.ledCount);
    ui->spinBrightness->setValue               (s.ledMaxBright);
    ui->checkLedReverseOrder->setChecked       (s.ledReversedOrder);
    ui->checkMatrixZigZag->setChecked          (s.ledMatrixZigZag);
    ui->spinMatrixX->setValue                  (s.ledMatrixXSize);
    ui->spinMatrixY->setValue                  (s.ledMatrixYSize);
    ui->spinMatrixTX->setValue                 (s.ledMatrixXTiles);
    ui->spinMatrixTY->setValue                 (s.ledMatrixYTiles);

    ui->checkArtnetEnabled->setChecked      (s.artNetEnabled);
    ui->spinStartUniverse->setValue         (s.startUniverse);
}

void editExtraSettingsGroupBox::sendSettings()
{

    extraSettings s;

    s.alivePin      = pin2GPIO(ui->comboAlivePin->currentText());
    s.ledPin        = pin2GPIO(ui->comboLedPin->currentText());
    s.ledClock      = pin2GPIO(ui->comboLedClock->currentText());
    s.sdaPin        = pin2GPIO(ui->comboSDA->currentText());
    s.sclPin        = pin2GPIO(ui->comboSCL->currentText());
    s.eqOut         = pin2GPIO(ui->comboEQOut->currentText());
    s.eqReset       = pin2GPIO(ui->comboEQReset->currentText());
    s.eqStrobe      = pin2GPIO(ui->comboEQStrobe->currentText());
    s.btn1Pin       = pin2GPIO(ui->comboButton1Pin->currentText());
    s.btn2Pin       = pin2GPIO(ui->comboButton2Pin->currentText());


    s.ledhwType          = (ledHardwareType)ui->comboLedHWType->currentIndex();
    s.gadgetType         = (ledGadgetType)ui->comboLedGadgetType->currentIndex();
    s.defaultAnimation   = (ledGadgetAnimations)ui->comboDefaultAnimation->currentIndex();
    s.defaultColor[0]    = ui->spinR->value();
    s.defaultColor[1]    = ui->spinG->value();
    s.defaultColor[2]    = ui->spinB->value();
    s.ledCount           = ui->spinLedCount->value();
    s.ledMaxBright       = ui->spinBrightness->value();
    s.ledReversedOrder   = ui->checkLedReverseOrder->isChecked();
    s.ledMatrixZigZag    = ui->checkMatrixZigZag->isChecked();
    s.ledMatrixXSize     = ui->spinMatrixX->value();
    s.ledMatrixYSize     = ui->spinMatrixY->value();
    s.ledMatrixXTiles    = ui->spinMatrixTX->value();
    s.ledMatrixYTiles    = ui->spinMatrixTY->value();

    s.artNetEnabled      = ui->checkArtnetEnabled->isChecked();
    s.startUniverse      = ui->spinStartUniverse->value();

    ledGadgetProtocolPacket p;
    p.cmd = cmdSetBright;
    p.floatVal0 = s.ledMaxBright;
    m_client->sendLedGadgetProtocol(p);

    m_client->sendExtraSettings(s);
}

void editExtraSettingsGroupBox::populateComboBoxes()
{
    QStringList pinlist;
    pinlist <<"None"<<"D0"<<"D1"<<"D2"<<"D3"<<"D4"<<"D5"<<"D6"<<"D7"<<"D8"<< "A0";
    ui->comboAlivePin->addItems(pinlist);
    ui->comboLedPin->addItems(pinlist);
    ui->comboLedClock->addItems(pinlist);
    ui->comboSDA->addItems(pinlist);
    ui->comboSCL->addItems(pinlist);
    ui->comboEQOut->addItems(pinlist);
    ui->comboEQReset->addItems(pinlist);
    ui->comboEQStrobe->addItems(pinlist);
    ui->comboButton1Pin->addItems(pinlist);
    ui->comboButton2Pin->addItems(pinlist);

    QStringList hwtypes;
    hwtypes << "hwWS2812Strip" << "hwWS2812RGBWStrip" << "hwWS2812Matrix" << "hwWS2801Strip" << "hwAPA102Strip" << "hwAPA102Matrix" ;
    ui->comboLedHWType->addItems(hwtypes);

    QStringList ledGadgetypes;
    ledGadgetypes << "gadgetNone" << "gadgetDMX" << "gadgetSpotLed" << "gadgetLedBar" << "gadgetLedCircle" << "gadgetLedWheel" << "gadgetledMatrix" << "gadgetledTree" << "gadgetLibreSaber";
    ui->comboLedGadgetType->addItems(ledGadgetypes);

    QStringList Animations;
    Animations    << "animationNone" << "animationDMX" << "animationFade" << "animationGlow" << "animationFlash" << "animationStrobe"
                  << "animationFlashOnPeak" << "animationSparks" << "animationCylon" << "animationChaoticLight"  << "animationRainbow"
                  << "animationVUMeter" << "animationVUMeterCentral" << "animationVUMeterHist" << "animationEQ"  << "animationEQCenter"
                  << "animationEQHist" << "animationScrollText" << "animationShowBPM" << "animationSpeed2Light"
                  << "animationSaberOn" "animationSaber" << "animationSaberOff";
    ui->comboDefaultAnimation->addItems(Animations);
}


void editExtraSettingsGroupBox::updateLedCount()
{
    if(!ui->gbMatrix->isVisible())
        return;

    int val = ui->spinMatrixY->value() * ui->spinMatrixX->value() *
            ui->spinMatrixTX->value() * ui->spinMatrixTY->value();

    ui->spinLedCount->setValue(val);
}

void editExtraSettingsGroupBox::enableMatrix(int index)
{

    if((ui->comboLedHWType->itemText(index) == "hwWS2812Matrix") ||
       (ui->comboLedHWType->itemText(index) == "hwAPA102Matrix") )
        ui->gbMatrix->show();
    else
        ui->gbMatrix->hide();
}
