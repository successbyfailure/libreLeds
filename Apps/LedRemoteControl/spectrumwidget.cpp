#include "spectrumwidget.h"
#include "ui_spectrumwidget.h"

spectrumWidget::spectrumWidget(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::spectrumWidget)
{
    ui->setupUi(this);
    this->setMinimumHeight(150);
    for(int i = 0 ; i<7 ; i++)
        m_spectrum.append(QVector<quint8>()<<0);
    m_vuLevel.append(0);
    m_blinkCounter = 255;
}

spectrumWidget::~spectrumWidget()
{
    delete ui;
}

void spectrumWidget::setClient(ledController* c)
{
    m_client = c;
    connect(c,SIGNAL(ledGadgetProtocol(ledGadgetProtocolPacket)),this,SLOT(processMsg(ledGadgetProtocolPacket)));
}

void spectrumWidget::processMsg(ledGadgetProtocolPacket p)
{
    if(p.cmd == msgPeak)
    {
        m_bpm.push_front(p.floatVal0);
        if(m_bpm.count() > 50)
            m_bpm.removeLast();
        m_blinkCounter = 0;
    }
    else if(p.cmd == msgVuLevel)
    {
        m_vuLevel.push_front(p.intVal0);

        m_spectrum[0].push_front(p.intVal1);
        m_spectrum[1].push_front(p.intVal2);
        m_spectrum[2].push_front(p.intVal3);
        m_spectrum[3].push_front(p.intVal4);
        m_spectrum[4].push_front(p.intVal5);
        m_spectrum[5].push_front(p.intVal6);
        m_spectrum[6].push_front(p.intVal7);

        if(m_vuLevel.count()  >50)
            m_vuLevel.removeLast();
        if(m_spectrum.count() >50)
            for(int i = 0 ; i < m_spectrum.size() ; i++)
                m_spectrum[i].removeLast();
    }
    else
        return;

    this->repaint();
}

void spectrumWidget::paintEvent(QPaintEvent *event)
{
    QColor backColor    (50,50,50);
    QColor outlineColor (90,90,90);
    QColor textColor    (0,0,200);

    QGroupBox::paintEvent(event);
    QPainter p(this);
    QRect rect = this->rect();
    rect.setSize(rect.size()-QSize(1,1));
    rect.setTop(15);

    quint16 hmargin = 0;
    quint16 vmargin = 0;

    QRect topRect = rect;
    hmargin = topRect.width()  *0.01;
    vmargin = topRect.height() *0.02;
    topRect.setTop   (rect.top()   +vmargin);
    topRect.setBottom(rect.bottom()*0.8);
    topRect.setLeft  (rect.left()  +hmargin);
    topRect.setRight (rect.right() -hmargin);

    QRect vuArea = rect;
    hmargin = vuArea.width()  *0.01;
    vmargin = vuArea.height() *0.02;
    vuArea.setBottom(rect.bottom()-vmargin);
    vuArea.setLeft  (rect.left()  +hmargin);
    vuArea.setRight (rect.right() -hmargin);
    vuArea.setTop   (topRect.bottom()+vmargin);

    QRect eqRect = topRect;
    hmargin = eqRect.width()  *0.01;
    vmargin = eqRect.height() *0.02;
    quint16 offsetEq = (eqRect.width() - (hmargin*2))*1/3;
    eqRect.setTop   (topRect.top()   +vmargin);
    eqRect.setBottom(topRect.bottom()-vmargin);
    eqRect.setLeft  (topRect.left()+offsetEq);
    eqRect.setRight (topRect.right() -hmargin);

    QRect leftRect = topRect;
    hmargin = leftRect.width()  *0.01;
    vmargin = leftRect.height() *0.02;

    leftRect.setTop   (topRect.top()   +vmargin);
    leftRect.setBottom(topRect.bottom()-vmargin);
    leftRect.setLeft  (topRect.left()  +hmargin);
    leftRect.setRight (topRect.left()+offsetEq -hmargin);

    QRect peakRect = leftRect;
    hmargin = leftRect.width()  *0.01;
    vmargin = leftRect.height() *0.02;

    peakRect.setTop   (leftRect.top()   +vmargin);
    peakRect.setBottom((leftRect.bottom()-(leftRect.height()*0.5))-vmargin);
    peakRect.setLeft  (leftRect.left()  +hmargin);
    peakRect.setRight (leftRect.right() -hmargin);

    QRect bpmRect = leftRect;
    hmargin = leftRect.width()  *0.01;
    vmargin = leftRect.height() *0.02;

    bpmRect.setTop   ((leftRect.top()+(leftRect.height()*0.5))+vmargin);
    bpmRect.setBottom(leftRect.bottom() -vmargin);
    bpmRect.setLeft  (leftRect.left()  +hmargin);
    bpmRect.setRight (leftRect.right() -hmargin);

    QVector<QRect> spectrumBars = splitRect(eqRect,7);


    p.setBrush(backColor);
    p.setPen(outlineColor);

    p.drawRect(rect);
    p.drawRect(topRect);
    p.drawRect(vuArea);
    p.drawRect(eqRect);
    p.drawRect(leftRect);
    p.drawRect(peakRect);
    p.drawRect(bpmRect);

    paintPeak (&p,peakRect);
    paintVuBar(&p,vuArea,m_vuLevel.first());

    for(quint16 i = 0 ; i < 7 ; i++)
    {
        p.setBrush(backColor);
        p.setPen  (outlineColor);
        p.drawRect(spectrumBars[i]);
        paintVuBar(&p,spectrumBars[i],m_spectrum[i].first());
    }
}
