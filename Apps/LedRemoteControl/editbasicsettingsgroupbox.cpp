#include "editbasicsettingsgroupbox.h"
#include "ui_editbasicsettingsgroupbox.h"

editBasicSettingsGroupBox::editBasicSettingsGroupBox(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::editBasicSettingsGroupBox)
{
    ui->setupUi(this);
    populateComboBoxes();
}

editBasicSettingsGroupBox::~editBasicSettingsGroupBox()
{
    delete ui;
}

void editBasicSettingsGroupBox::setClient(protocolClient* c)
{
    m_client = c;

    connect(c   ,SIGNAL(basicSettingsReceived(basicSettings)),
            this,SLOT  (loadSettings(basicSettings)));

    loadSettings();
    connect(c,SIGNAL(connected(bool))   ,this,SLOT(setEnabled(bool)));
    connect(c,SIGNAL(disconnected(bool)),this,SLOT(setDisabled(bool)));
    connect(ui->btnSave,SIGNAL(clicked(bool)),c,SLOT(saveSettings()));
    connect(ui->btnSend,SIGNAL(clicked(bool)),this,SLOT(sendSettings()));
}

void editBasicSettingsGroupBox::loadSettings()
{
    loadSettings(m_client->getBasicSettings());
}

void editBasicSettingsGroupBox::loadSettings(basicSettings s)
{
    ui->editID->setText                     (s.id);
    ui->spinDelayTime->setValue             (s.delayTime);
    ui->spinWatchdog->setValue              (s.watchdogTime);
    ui->spinHfTimer->setValue               (s.highFreqCycleMs);
    ui->spinMfTimer->setValue               (s.medFreqCycleMs);
    ui->spinLfTimer->setValue               (s.lowFreqCycleMs);

    ui->spinLocalPort->setValue             (s.localPort);
    ui->spinHttpUpdaterPort->setValue       (s.httpUpdatePort);
    ui->spinHttpPort->setValue              (s.httpPort);

    ui->checkHttpUpdateEnabled->setChecked  (s.httpUpdaterEnabled);
    ui->checkHttpServerEnabled->setChecked  (s.httpServerEnabled);
    ui->checkMdnsEnabled->setChecked        (s.MdnsEnabled);
    ui->checkSerialClient->setChecked       (s.serialClient);
    ui->checkTCPCLientEnabled->setChecked   (s.localPortEnabled);

    ui->comboWifiMode->setCurrentIndex      (s.wifiMode);

    ui->editEssid->setText                  (s.wifiESSID);
    ui->editPasswd->setText                 (s.wifiPasswd);
    ui->checkremoteEnabled->setChecked      (s.autoConnectRemote);
    ui->editRemoteHost->setText             (QString(s.remoteHost));
    ui->spinRemotePort->setValue            (s.remotePort);

    ui->checkAllowOverride->setChecked      (s.allowWifiOverride);
    ui->editOEssid->setText                 (s.wifiOverrideESSID);
    ui->editOPasswd->setText                (s.wifiOverridePasswd);
    ui->checkOremoteEnabled->setChecked     (s.overrideAutoConnectRemote);
    ui->editORemoteHost->setText            (QString(s.overrideRemoteHost));
    ui->spinORemotePort->setValue           (s.overrideRemotePort);

    ui->checkStaticIP->setChecked           (s.staticIP);
    ui->spinIP0->setValue                   (s.IP[0]);
    ui->spinIP1->setValue                   (s.IP[1]);
    ui->spinIP2->setValue                   (s.IP[2]);
    ui->spinIP3->setValue                   (s.IP[3]);
    ui->spinNetMaks0->setValue              (s.subnet[0]);
    ui->spinNetMaks1->setValue              (s.subnet[1]);
    ui->spinNetMaks2->setValue              (s.subnet[2]);
    ui->spinNetMaks3->setValue              (s.subnet[3]);
}

void editBasicSettingsGroupBox::sendSettings()
{

    basicSettings s;

    for(int i = 0 ; i < 20 ; i++)
        if(i < ui->editID->text().trimmed().toLatin1().count())
            s.id[i] = ui->editID->text().trimmed().toLatin1().data()[i];
        else
            s.id[i] = 0;

    s.delayTime       = ui->spinDelayTime->value() ;
    s.watchdogTime    = ui->spinWatchdog->value() ;
    s.highFreqCycleMs = ui->spinHfTimer->value();
    s.medFreqCycleMs  = ui->spinMfTimer->value();
    s.lowFreqCycleMs  = ui->spinLfTimer->value();
    s.localPort       = ui->spinLocalPort->value();
    s.httpUpdatePort  = ui->spinHttpUpdaterPort->value();
    s.httpPort        = ui->spinHttpPort->value();

    s.httpServerEnabled  = ui->checkHttpServerEnabled->isChecked();
    s.httpUpdaterEnabled = ui->checkHttpUpdateEnabled->isChecked();
    s.MdnsEnabled        = ui->checkMdnsEnabled->isChecked();
    s.serialClient       = ui->checkSerialClient->isChecked();
    s.localPortEnabled   = ui->checkTCPCLientEnabled->isChecked();

    s.wifiMode          = (wMode)ui->comboWifiMode->currentIndex();

    for(int i = 0 ; i < 20 ; i++)
        if(i < ui->editEssid->text().count())
            s.wifiESSID[i] = ui->editEssid->text().trimmed().toLatin1().data()[i];
        else
            s.wifiESSID[i] = 0;

    for(int i = 0 ; i < 20 ; i++)
        if(i < ui->editPasswd->text().count())
            s.wifiPasswd[i] = ui->editPasswd->text().trimmed().toLatin1().data()[i];
        else
            s.wifiPasswd[i] = 0;

    s.autoConnectRemote  = ui->checkremoteEnabled->isChecked();

    for(int i = 0 ; i < 20 ; i++)
        if(i < ui->editRemoteHost->text().trimmed().toLatin1().count())
            s.remoteHost[i] = ui->editRemoteHost->text().trimmed().toLatin1().data()[i];
        else
            s.remoteHost[i] = 0;
    s.remotePort         = ui->spinRemotePort->value();

    s.allowWifiOverride = ui->checkAllowOverride->isChecked();
    for(int i = 0 ; i < 20 ; i++)
        if(i < ui->editOEssid->text().count())
            s.wifiOverrideESSID[i] = ui->editOEssid->text().trimmed().toLatin1().data()[i];
        else
            s.wifiOverrideESSID[i] = 0;

    for(int i = 0 ; i < 20 ; i++)
        if(i < ui->editOPasswd->text().count())
            s.wifiOverridePasswd[i] = ui->editOPasswd->text().trimmed().toLatin1().data()[i];
        else
            s.wifiOverridePasswd[i] = 0;

    s.overrideAutoConnectRemote  = ui->checkOremoteEnabled->isChecked();

    for(int i = 0 ; i < 20 ; i++)
        if(i < ui->editORemoteHost->text().trimmed().toLatin1().count())
            s.overrideRemoteHost[i] = ui->editORemoteHost->text().trimmed().toLatin1().data()[i];
        else
            s.overrideRemoteHost[i] = 0;

    s.overrideRemotePort         = ui->spinORemotePort->value();


    s.staticIP  = ui->checkStaticIP->isChecked();
    s.IP[0]     = ui->spinIP0->value();
    s.IP[1]     = ui->spinIP1->value();
    s.IP[2]     = ui->spinIP2->value();
    s.IP[3]     = ui->spinIP3->value();
    s.subnet[0] = ui->spinNetMaks0->value();
    s.subnet[1] = ui->spinNetMaks1->value();
    s.subnet[2] = ui->spinNetMaks2->value();
    s.subnet[3] = ui->spinNetMaks3->value();

    m_client->sendBasicSettings(s);
}

void editBasicSettingsGroupBox::populateComboBoxes()
{
    QStringList wifimodes;
    wifimodes << "wifiAP" << "wifiClient" << "wifiMesh";
    ui->comboWifiMode->addItems(wifimodes);
}
