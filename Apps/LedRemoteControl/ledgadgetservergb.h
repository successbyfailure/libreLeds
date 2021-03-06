#ifndef LEDGADGETSERVERGB_H
#define LEDGADGETSERVERGB_H

#include <QGroupBox>
#include "ledgadgetclientwidget.h"
#include "ledcontrolerserver.h"

namespace Ui {
class ledGadgetServerGB;
}

class ledGadgetServerGB : public QGroupBox
{
    Q_OBJECT

public:
    explicit ledGadgetServerGB(QWidget *parent = 0);
    ~ledGadgetServerGB();

private:
    Ui::ledGadgetServerGB *ui;
    ledGadgetGroupBox*     m_displayedWidget;
    ledControlerServer     m_server;

protected slots:
    void addWidget(ledGadgetClientWidget *c = 0);
    void addClient(ledController* lc);
    void showClientWidget(ledGadgetGroupBox* w);
    void hideClientWidget();
    void shownWidgetDestroyed();
};

#endif // LEDGADGETSERVERGB_H
