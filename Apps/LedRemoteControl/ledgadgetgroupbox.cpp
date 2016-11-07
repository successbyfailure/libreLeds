#include "ledgadgetgroupbox.h"
#include "ui_ledgadgetgroupbox.h"

ledGadgetGroupBox::ledGadgetGroupBox(ledController *l, QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ledGadgetGroupBox)
{
    m_ledGadget = l;
    ui->setupUi(this);
    ui->gbBasicSettings->hide();
    ui->gbExtraSettings->hide();
    ui->gbEffects->hide();
    ui->gbLamp->hide();
    ui->connectW->hide();

    connect(ui->btnShowBasicSettings,SIGNAL(toggled(bool)),ui->gbBasicSettings,SLOT(setVisible(bool)));
    connect(ui->btnShowExtraSettings,SIGNAL(toggled(bool)),ui->gbExtraSettings,SLOT(setVisible(bool)));
    connect(ui->btnShowConnect      ,SIGNAL(toggled(bool)),ui->connectW       ,SLOT(setVisible(bool)));
    connect(ui->btnShowLamp         ,SIGNAL(toggled(bool)),ui->gbLamp         ,SLOT(setVisible(bool)));
    connect(ui->btnShowEffects      ,SIGNAL(toggled(bool)),ui->gbEffects      ,SLOT(setVisible(bool)));

    connect(l,SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));

    ui->gbBasicSettings->setClient(l);
    ui->gbExtraSettings->setClient(l);
    ui->connectW->setClient(l);
    ui->gbLamp->setClient(l);
    ui->gbEffects->setClient(l);
}

ledGadgetGroupBox::~ledGadgetGroupBox()
{
    delete ui;
}
