#ifndef ANIMATIONFRAME_H
#define ANIMATIONFRAME_H

#include <QFrame>
#include "ledcontroller.h"

namespace Ui {
class animationFrame;
}

class animationFrame : public QFrame
{
    Q_OBJECT

public:
    explicit animationFrame(ledGadgetAnimations a, QString name, ledController* c, QWidget *parent = 0);
    ~animationFrame();


private:
    Ui::animationFrame *ui;
    ledController*      m_ledClient;
    ledGadgetAnimations m_animation;
    QString             m_animationName;

    void initUI();
public slots:
    void setAnimation();

};

#endif // ANIMATIONFRAME_H
