#include "ledgadgetclientwidget.h"
#include "ui_ledgadgetclientwidget.h"

ledGadgetClientWidget::ledGadgetClientWidget(ledController* l, QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ledGadgetClientWidget)
{
    ui->setupUi(this);
    if(l != 0)
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
    connect(l,SIGNAL(ready()),this,SLOT(refreshData()));
    refreshData();
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

void ledGadgetClientWidget::refreshData()
{
    this->setTitle(m_ledGadget->getBasicSettings().id);
    extraSettings s = m_ledGadget->getExtraSettings();

    if      (s.ledhwType == hwAPA102Strip)
        ui->labelHwType->setPixmap(QPixmap(":/icons/icons/APA102Strip.png"));

    else if (s.ledhwType == hwAPA102Matrix)
        ui->labelHwType->setPixmap(QPixmap(":/icons/icons/APA102Matrix.png"));

    else if (s.ledhwType == hwWS2812Strip)
        ui->labelHwType->setPixmap(QPixmap(":/icons/icons/ws2812Strip.png"));

    else if (s.ledhwType == hwWS2812Matrix)
        ui->labelHwType->setPixmap(QPixmap(":/icons/icons/ws2812Matrix.png"));

    else
        ui->labelHwType->setPixmap(QPixmap(":/icons/icons/unknown.png"));

}
