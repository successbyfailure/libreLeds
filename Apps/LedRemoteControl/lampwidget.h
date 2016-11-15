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
    void setClient(ledController* l);

private:
    Ui::LampWidget *ui;
    ledController*  m_client;
    QColor          m_color;

signals:
    void sliderValueuint(quint8);
protected slots:
    void showSelector();
    void setColor(QColor);
    void sliderValue(int);
};

#endif // LAMPWIDGET_H
