#ifndef SPECTRUMWIDGET_H
#define SPECTRUMWIDGET_H

#include <QGroupBox>
#include "ledcontroller.h"
#include <QPainter>


namespace Ui {
class spectrumWidget;
}

class spectrumWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit spectrumWidget(QWidget *parent = 0);
    ~spectrumWidget();
    void setClient(ledController* c);

private:
    Ui::spectrumWidget       *ui;
    ledController*            m_client;
    QVector<float>            m_bpm;
    QVector<quint8>           m_vuLevel;
    QVector<QVector<quint8> > m_spectrum;
    quint8                    m_blinkCounter;

    void           paintEvent(QPaintEvent *event);

    QVector<QRect> splitRect(QRect& r, quint16 count)
    {
        QVector<QRect> result;
        if(r.height()>r.width())
        {

        }else{
            quint16 vmargin         = r.height() *0.01;
            quint16 hmargin         = r.width()  *0.01;
            quint16 effectiveSize   = r.width()-(hmargin*(count+1));
            quint16 barsize         = effectiveSize/count;
            quint16 centralBarsize  = barsize + (effectiveSize%count);
            quint16 centralBarIndex = count/2;

            quint16 offset = 0;
            for(quint16 c = 0 ; c < count ; c++)
            {
                quint16 bar = 0;
                offset  += hmargin;

                if(c == centralBarIndex)
                    bar = centralBarsize;
                else
                    bar = barsize;

                QRect sr = r;
                sr.setTop   (r.top()    +vmargin);
                sr.setBottom(r.bottom() -vmargin);
                sr.setLeft  (r.left()+offset);
                sr.setRight (sr.left()+bar);
                offset += bar;
                result.append(sr);
            }
        }
        return result;
    }

    void paintVuBar(QPainter* p, QRect r,quint8 val,quint8 brickSize = 8,QColor OColor = QColor(100,100,100), QColor HColor = QColor(255,0,0),QColor MColor = QColor(255,255,0),QColor LColor = QColor(0,255,0),QColor IColor = QColor(0,0,255))
    {
        if(val > 100) val = 100;
        quint16 vmargin       = r.height() * 0.05;
        quint16 hmargin       = r.width()  * 0.05;
        quint16 bricks        = r.height()/(brickSize+vmargin);

        quint16 litBricks = bricks *(val/100.0f);

        quint16 offset = r.bottom();
        for(quint16 i = 0 ; i < litBricks ; i++)
        {
            quint8 power = (i*val)/litBricks;

            p->setPen(OColor);

            if     (power < 25)
                p->setBrush(IColor);
            else if(power < 65)
                p->setBrush(LColor);
            else if(power < 80)
                p->setBrush(MColor);
            else
                p->setBrush(HColor);

            offset -= vmargin;
            offset -= brickSize;
            QRect brickRect = r;

            brickRect.setBottom(offset);
            brickRect.setTop   (brickRect.bottom() -brickSize);
            brickRect.setLeft  (r.left()  +hmargin);
            brickRect.setRight (r.right() -hmargin);

            p->drawRect(brickRect);
        }
    }

    void paintPeak(QPainter* p,QRect r,QColor outline = QColor(100,100,100),QColor blinkColor = QColor(255,0,0))
    {
        p->setPen(outline);
        QColor c = blinkColor;
        c.setAlpha(m_blinkCounter);
        p->setBrush(c);
        p->drawEllipse(r);
        if(m_blinkCounter < 255)
            m_blinkCounter++;
    }

public slots:
    void processMsg(ledGadgetProtocolPacket p);
};

#endif // SPECTRUMWIDGET_H
