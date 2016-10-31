#include "sensorswidget.h"
#include "ui_sensorswidget.h"

sensorsWidget::sensorsWidget(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::sensorsWidget)
{
    ui->setupUi(this);
    addGraphs();
    m_samplesPerSec = 0;
    connect(&m_sampleTimer,SIGNAL(timeout()),this,SLOT(countReads()));
    m_sampleTimer.start(1000);
}

sensorsWidget::~sensorsWidget()
{
    delete ui;
}

void sensorsWidget::sensorRead(float Gx, float Gy, float Gz, float AcX, float AcY, float AcZ, float angleY, float rangleY, float Temp, int vuLevel)
{
    m_samplesPerSec++;

    ui->dial_gx->setValue(Gx);
    ui->valueLabel_gx->setText(QString::number(Gx));
    m_graph_gx->appendData(QVector<double>() << Gx);

    ui->dial_gy->setValue(Gy);
    ui->valueLabel_gy->setText(QString::number(Gy));
    m_graph_gy->appendData(QVector<double>() << Gy);

    ui->dial_gz->setValue(Gz);
    ui->valueLabel_gz->setText(QString::number(Gz));
    m_graph_gz->appendData(QVector<double>() << Gz);

    ui->progressBar_acx->setValue(AcX);
    ui->valueLabel_acx->setText(QString::number(AcX));
    m_graph_accx->appendData(QVector<double>() << AcX);

    ui->progressBar_acy->setValue(AcY);
    ui->valueLabel_acy->setText(QString::number(AcY));
    m_graph_accy->appendData(QVector<double>() << AcY);

    ui->progressBar_acz->setValue(AcZ);
    ui->valueLabel_acz->setText(QString::number(AcZ));
    m_graph_accz->appendData(QVector<double>() << AcZ);


    ui->dial_angleY->setValue(angleY);
    ui->valueLabel_angleY->setText(QString::number(angleY));

    ui->dial_rangleY->setValue(rangleY);
    ui->valueLabel_rAngleY->setText(QString::number(rangleY));

    ui->progressBar_temp->setValue(Temp);
    ui->valueLabel_temp->setText(QString::number(Temp));

    ui->progressBar_vuLevel->setValue(vuLevel);
    ui->valueLabel_vuLevel->setText(QString::number(vuLevel));
    m_graph_vuMeter->appendData(QVector<double>() << vuLevel);
}


void sensorsWidget::addGraphs()
{
    m_graph_gx = new gSimpleGraph(ui->graficos,QVector<QString>()<<QString("Gx"),500);
    m_graph_gx->appendData(QVector<double>()<< 0);
    m_graph_gx->setMinimumHeight(80);
    ui->graficos->layout()->addWidget(m_graph_gx);

    m_graph_gy = new gSimpleGraph(ui->graficos,QVector<QString>()<<QString("Gy"),500);
    m_graph_gy->appendData(QVector<double>()<< 0);
    m_graph_gy->setMinimumHeight(80);
    ui->graficos->layout()->addWidget(m_graph_gy);

    m_graph_gz = new gSimpleGraph(ui->graficos,QVector<QString>()<<QString("Gz"),500);
    m_graph_gz->appendData(QVector<double>()<< 0);
    m_graph_gz->setMinimumHeight(80);
    ui->graficos->layout()->addWidget(m_graph_gz);


    m_graph_accx = new gSimpleGraph(ui->graficos,QVector<QString>()<<QString("accX"),500);
    m_graph_accx->appendData(QVector<double>()<< 0);
    m_graph_accx->setMinimumHeight(80);
    ui->graficos->layout()->addWidget(m_graph_accx);

    m_graph_accy = new gSimpleGraph(ui->graficos,QVector<QString>()<<QString("accY"),500);
    m_graph_accy->appendData(QVector<double>()<< 0);
    m_graph_accy->setMinimumHeight(80);
    ui->graficos->layout()->addWidget(m_graph_accy);

    m_graph_accz = new gSimpleGraph(ui->graficos,QVector<QString>()<<QString("accZ"),500);
    m_graph_accz->appendData(QVector<double>()<< 0);
    m_graph_accz->setMinimumHeight(80);
    ui->graficos->layout()->addWidget(m_graph_accz);


    m_graph_vuMeter = new gSimpleGraph(ui->graficos,QVector<QString>()<<QString("VuMeter"),500);
    m_graph_vuMeter->appendData(QVector<double>()<< 0);
    m_graph_vuMeter->setMinimumHeight(80);
    ui->graficos->layout()->addWidget(m_graph_vuMeter);
}

void sensorsWidget::countReads()
{
    QString txt;
    txt.append("Sensors: ");
    txt.append(QString::number(m_samplesPerSec));
    txt.append(" Reads/s");
    this->setTitle(txt);
    m_samplesPerSec = 0;
    m_sampleTimer.start();
}
