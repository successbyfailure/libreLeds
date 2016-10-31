#include "connectwidget.h"
#include "ui_connectwidget.h"

connectWidget::connectWidget(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::connectWidget)
{
    ui->setupUi(this);
    clientDisconnected();

    connect(ui->btnConnect    ,SIGNAL(clicked(bool)),this,SLOT(connectClient()));
    connect(ui->btnDisconnect ,SIGNAL(clicked(bool)),this,SLOT(disconnectClient()));
    connect(ui->btnShowCommands,SIGNAL(toggled(bool)),ui->basicCommandsgb,SLOT(setVisible(bool)));
    ui->basicCommandsgb->hide();
    ui->btnShowCommands->hide();
}

void connectWidget::setClient(protocolClient *c)
{
    m_client = c;
    ui->basicCommandsgb->setProtocolClient(c);
    connect(c,SIGNAL(tx()),ui->txWidget,SLOT(tx()));
    connect(c,SIGNAL(rx()),ui->txWidget,SLOT(rx()));
    connect(c,SIGNAL(disconnected()),this,SLOT(clientDisconnected()));
    connect(c,SIGNAL(connected())   ,this,SLOT(clientConnected()));
    connect(c,SIGNAL(connecting())  ,this,SLOT(clientConnecting()));
    connect(c,SIGNAL(ready())       ,this,SLOT(clientReady()));
    connect(c,SIGNAL(error()),this  ,SLOT(clientError()));
}

connectWidget::~connectWidget()
{
    delete ui;
}

void connectWidget::clientDisconnected()
{
    ui->tabWidget->setEnabled(true);
    ui->tabWidget->show();
    ui->txWidget->hide();
    ui->btnDisconnect->hide();
    ui->btnConnect->show();
    ui->btnShowCommands->hide();
    emit disconnected();
    emit status(false);
}

void connectWidget::clientConnecting()
{
    ui->btnDisconnect->setIcon(QIcon(":/icons/icons/connected-icon.png"));
    ui->tabWidget->setEnabled(false);
    ui->tabWifi->show();
    ui->txWidget->hide();
    ui->btnDisconnect->show();
    ui->btnConnect->hide();
}

void connectWidget::clientConnected()
{
    ui->tabWidget->setEnabled(true);
    ui->tabWidget->hide();
    ui->txWidget->show();
    ui->btnDisconnect->show();
    ui->btnConnect->hide();
    ui->btnShowCommands->show();
}

void connectWidget::clientReady()
{
    emit ready();
    emit status(true);
}

void connectWidget::clientError()
{
    clientDisconnected();
    ui->btnDisconnect->setIcon(QIcon(":/icons/icons/connect-error.png"));
    ui->btnDisconnect->show();
}

void connectWidget::connectClient()
{
    if(ui->tabWifi->isActiveWindow())
    {
        m_client->connectToHost(ui->editIP->text(),ui->editPort->text().toInt());
    }
    else if(ui->tabSerial->isActiveWindow())
    {
        //m_client->OpenSerial();
    }

}

void connectWidget::disconnectClient()
{
    m_client->close();
}

void connectWidget::socketError(QAbstractSocket::SocketError e)
{
    qDebug() << "Socket Error" << e;
    m_client->close();
    clientDisconnected();
}
