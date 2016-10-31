#include "simpleprotocolcommandsgroupbox.h"
#include "ui_simpleprotocolcommandsgroupbox.h"

simpleProtocolCommandsGroupBox::simpleProtocolCommandsGroupBox(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::simpleProtocolCommandsGroupBox)
{
    ui->setupUi(this);
}

simpleProtocolCommandsGroupBox::~simpleProtocolCommandsGroupBox()
{
    delete ui;
}

void simpleProtocolCommandsGroupBox::setProtocolClient(protocolClient *c)
{
    //connect(c,SIGNAL(ready())           ,this,SLOT(show()));
    //connect(c,SIGNAL(disconnected())    ,this,SLOT(hide()));

    connect(ui->btnReload,  SIGNAL(clicked(bool)),c,SLOT(reloadSettings()));
    connect(ui->btnSave,    SIGNAL(clicked(bool)),c,SLOT(saveSettings()));
    connect(ui->btnFactory, SIGNAL(clicked(bool)),c,SLOT(factoryReset()));
    connect(ui->btnGet,     SIGNAL(clicked(bool)),c,SLOT(getSettings()));
    connect(ui->btnPing,    SIGNAL(clicked(bool)),c,SLOT(ping()));
    connect(ui->btnReset,   SIGNAL(clicked(bool)),c,SLOT(reset()));
}
