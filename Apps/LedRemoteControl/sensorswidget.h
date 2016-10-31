#ifndef SENSORSWIDGET_H
#define SENSORSWIDGET_H

#include <QGroupBox>
#include "socketclient.h"
#include "gsimplegraph.h"
#include <QTimer>

namespace Ui {
class sensorsWidget;
}

class sensorsWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit sensorsWidget(QWidget *parent = 0);
    ~sensorsWidget();
    void setClient(socketClient* c)
    {
        m_client = c;
        connect(c,SIGNAL(sensorRead(float,float,float,float,float,float,float,float,float,int)),this,SLOT(sensorRead(float,float,float,float,float,float,float,float,float,int)));
    }
private:
    Ui::sensorsWidget   *ui;
    socketClient*       m_client;
    QTimer              m_sampleTimer;
    int                 m_samplesPerSec;
    gSimpleGraph*       m_graph_gx;
    gSimpleGraph*       m_graph_gy;
    gSimpleGraph*       m_graph_gz;
    gSimpleGraph*       m_graph_accx;
    gSimpleGraph*       m_graph_accy;
    gSimpleGraph*       m_graph_accz;
    gSimpleGraph*       m_graph_vuMeter;

    void addGraphs();

protected slots:
    void sensorRead(float Gx, float Gy, float Gz, float AcX, float AcY, float AcZ,float angleY,float rangleY, float Temp, int vuLevel);
    void countReads();
};

#endif // SENSORSWIDGET_H
