#include "connectwidget.h"
#include "ui_connectwidget.h"

connectWidget::connectWidget(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::connectWidget)
{
    ui->setupUi(this);

    populateSerialPorts();

    clientDisconnected();

    connect(ui->btnConnect    ,SIGNAL(clicked(bool)),this,SLOT(connectClient()));
    connect(ui->btnDisconnect ,SIGNAL(clicked(bool)),this,SLOT(disconnectClient()));
    connect(ui->btnShowCommands,SIGNAL(toggled(bool)),ui->basicCommandsgb,SLOT(setVisible(bool)));

    connect(ui->btnRefreshSerial,SIGNAL(clicked(bool)),this,SLOT(populateSerialPorts()));
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
    if(c->isOpen())
        clientConnected();
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
    if(ui->tabWifi->isVisible())
    {
        m_client->connectToHost(ui->editIP->text(),ui->editPort->text().toInt());
    }
    else if(ui->tabSerial->isVisible())
    {
        QString port = ui->comboDevice->currentText();
        QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
        for(int i = 0 ; i < ports.size() ; i++)
            if(port == ports[i].portName())
            {
                m_client->OpenSerial(ports[i],ui->comboSpeed->currentText().toInt());
                return;
            }
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


void connectWidget::populateSerialPorts()
{
    ui->comboDevice->clear();
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for(int i = 0 ; i < ports.size() ; i++)
        ui->comboDevice->addItem(ports[i].portName());
}
