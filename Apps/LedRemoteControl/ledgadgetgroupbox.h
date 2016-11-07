#ifndef LEDGADGETGROUPBOX_H
#define LEDGADGETGROUPBOX_H

#include <QGroupBox>
#include "ledcontroller.h"

namespace Ui {
class ledGadgetGroupBox;
}

class ledGadgetGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit ledGadgetGroupBox(ledController* l, QWidget *parent = 0);
    ~ledGadgetGroupBox();

private:
    Ui::ledGadgetGroupBox *ui;
    ledController*         m_ledGadget;

protected slots:

};

#endif // LEDGADGETGROUPBOX_H
