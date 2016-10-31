#ifndef ANIMATIONSELECTGROUPBOX_H
#define ANIMATIONSELECTGROUPBOX_H

#include <QGroupBox>
#include "ledcontroller.h"
#include "QToolButton"
#include "animationframe.h"

namespace Ui {
class animationSelectGroupbox;
}

class animationSelectGroupbox : public QGroupBox
{
    Q_OBJECT

public:
    explicit animationSelectGroupbox(QWidget *parent = 0);
    ~animationSelectGroupbox();
    void setClient(ledController* c);

private:
    Ui::animationSelectGroupbox *ui;
    ledController*  m_client;
    bool            m_populated;

    QVector<animationFrame*> m_widgets;
public slots:
    void populate();
    void populate(ledGadgetAnimationListPacket p);
    void reload();
};

#endif // ANIMATIONSELECTGROUPBOX_H
