#include "ledgadgetclientwidget.h"
#include "ui_ledgadgetclientwidget.h"

ledGadgetClientWidget::ledGadgetClientWidget(ledController* l, QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ledGadgetClientWidget)
{
    ui->setupUi(this);
    if(l)
        setClient(l);
    else
        setClient(new ledController());

    connect(ui->btnFlash,SIGNAL(clicked(bool)),this,SLOT(flash()));
    connect(ui->btnView,SIGNAL(clicked(bool)),this,SLOT(view()));
    connect(ui->btnRemove,SIGNAL(clicked(bool)),this,SLOT(remove()));
}

ledGadgetClientWidget::~ledGadgetClientWidget()
{
    disconnect(m_ledGadget,SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
    delete ui;
    m_ledGadget->deleteLater();
}


void ledGadgetClientWidget::setClient(ledController* l)
{
    m_ledGadget = l;
    connect(l,SIGNAL(tx()),ui->txWidget,SLOT(tx()));
    connect(l,SIGNAL(rx()),ui->txWidget,SLOT(rx()));
    connect(l,SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));
}

void ledGadgetClientWidget::view()
{
    ledGadgetGroupBox* l = new ledGadgetGroupBox(m_ledGadget);
    emit showWidget(l);
}

void ledGadgetClientWidget::flash()
{
    m_ledGadget->flash();
}

void ledGadgetClientWidget::remove()
{
    this->deleteLater();
}
