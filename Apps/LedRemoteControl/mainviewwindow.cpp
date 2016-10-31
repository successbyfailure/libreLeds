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
    connect(ui->connectW,SIGNAL(status(bool)),ui->tabWidget,SLOT(setEnabled(bool)));
    ui->gbBasicSettings->setEnabled(false);
    ui->gbExtraSettings->setEnabled(false);
    ui->gbEffects->setEnabled(false);
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

