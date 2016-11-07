#ifndef LAMPWIDGET_H
#define LAMPWIDGET_H

#include <QGroupBox>
#include "ledcontroller.h"

namespace Ui {
class LampWidget;
}

class LampWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit LampWidget(QWidget *parent = 0);
    ~LampWidget();
    void setClient(ledController* l){m_client = l;}

private:
    Ui::LampWidget *ui;
    ledController*  m_client;
};

#endif // LAMPWIDGET_H
