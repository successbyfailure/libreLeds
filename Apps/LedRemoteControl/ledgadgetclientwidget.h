#ifndef LEDGADGETCLIENTWIDGET_H
#define LEDGADGETCLIENTWIDGET_H

#include <QGroupBox>
#include "ledcontroller.h"
#include "ledgadgetgroupbox.h"

namespace Ui {
class ledGadgetClientWidget;
}

class ledGadgetClientWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit ledGadgetClientWidget(ledController* l = 0,QWidget *parent = 0);
    ~ledGadgetClientWidget();

private:
    Ui::ledGadgetClientWidget *ui;
    ledController*            m_ledGadget;

signals:
    void showWidget(ledGadgetGroupBox* w);
protected slots:
    void setClient(ledController* l);
    void view();
    void flash();
    void remove();
    void refreshData();
};

#endif // LEDGADGETCLIENTWIDGET_H
