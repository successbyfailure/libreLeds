#include "lampwidget.h"
#include "ui_lampwidget.h"

LampWidget::LampWidget(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::LampWidget)
{
    ui->setupUi(this);
}

LampWidget::~LampWidget()
{
    delete ui;
}
