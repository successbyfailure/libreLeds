#include "lampwidget.h"
#include "ui_lampwidget.h"
#include <QColorDialog>

LampWidget::LampWidget(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::LampWidget)
{
    ui->setupUi(this);
    m_color = QColor(50,50,50);
    showSelector();
}

LampWidget::~LampWidget()
{
    delete ui;
}

void LampWidget::setClient(ledController *l)
{
    m_client = l;
    ui->sliderBright->setValue(m_client->getExtraSettings().ledMaxBright*255);
    connect(ui->sliderBright,SIGNAL(valueChanged(int)),l,SLOT(setBrightness(quint8)));
    connect(ui->btnOff,SIGNAL(clicked(bool)),l,SLOT(fade()));
}

void LampWidget::showSelector()
{
    QColorDialog* d = new QColorDialog(m_color);
    ui->colorSelectorLy->addWidget(d);
    connect(d,SIGNAL(currentColorChanged(QColor)),this,SLOT(setColor(QColor)));
    connect(d,SIGNAL(destroyed(QObject*)),this,SLOT(showSelector()));
    connect(d,SIGNAL(accepted()),d,SLOT(show()));
    d->show();
}

void LampWidget::setColor(QColor c)
{
    m_color = c;
    m_client->setColor(c);
}
