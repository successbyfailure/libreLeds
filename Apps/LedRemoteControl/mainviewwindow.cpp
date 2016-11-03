#include "mainviewwindow.h"
#include "ui_mainviewwindow.h"

mainViewWindow::mainViewWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainViewWindow)
{
    ui->setupUi(this);
    m_ledClient = 0;
    m_deleteClient = true;
    setClient(new ledController(),true);
    ui->gbBasicSettings->setEnabled(false);
    ui->gbExtraSettings->setEnabled(false);
    ui->gbEffects->setEnabled(false);

    QToolButton* bConn = new QToolButton(this);
    ui->mainToolBar->addWidget(bConn);
    bConn->setText("Connect");
    bConn->setCheckable(true);
    bConn->setChecked(true);
    connect(bConn,SIGNAL(clicked(bool)),ui->connectW,SLOT(setVisible(bool)));

    QToolButton* bbset = new QToolButton(this);
    ui->mainToolBar->addWidget(bbset);
    bbset->setText("ConnSettings");
    bbset->setCheckable(true);
    connect(bbset,SIGNAL(clicked(bool)),ui->gbBasicSettings,SLOT(setVisible(bool)));

    QToolButton* beset = new QToolButton(this);
    ui->mainToolBar->addWidget(beset);
    beset->setText("LedSettings");
    beset->setCheckable(true);
    connect(beset,SIGNAL(clicked(bool)),ui->gbExtraSettings,SLOT(setVisible(bool)));

    QToolButton* bvU = new QToolButton(this);
    ui->mainToolBar->addWidget(bvU);
    bvU->setText("vuMeter");
    bvU->setCheckable(true);
    connect(bvU,SIGNAL(clicked(bool)),ui->gbVuLevels,SLOT(setVisible(bool)));

    ui->gbBasicSettings->hide();
    ui->gbExtraSettings->hide();
    ui->gbVuLevels->hide();
}

mainViewWindow::~mainViewWindow()
{
    if(m_deleteClient)
        m_ledClient->deleteLater();
    delete ui;
}

void mainViewWindow::setClient(ledController *client, bool deleteObj)
{
    m_deleteClient = deleteObj;
    m_ledClient = client;
    ui->connectW->setClient(client);
    ui->gbBasicSettings->setClient(client);
    ui->gbExtraSettings->setClient(client);
    ui->gbEffects->setClient(client);
    ui->gbVuLevels->setClient(client);
}

