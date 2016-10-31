#include "animationframe.h"
#include "ui_animationframe.h"

animationFrame::animationFrame(ledGadgetAnimations a,QString name ,ledController *c, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::animationFrame)
{
    ui->setupUi(this);
    m_animation = a;
    m_ledClient = c;
    m_animationName = name;
    initUI();
}

animationFrame::~animationFrame()
{
    delete ui;
}

void animationFrame::initUI()
{
    ui->btn->setText(m_animationName);
    ui->slider->hide();
    ui->label->hide();
    connect(ui->btn,SIGNAL(clicked(bool)),this,SLOT(setAnimation()));

}

void animationFrame::setAnimation()
{
    ledGadgetProtocolPacket p;
    p.cmd = cmdSetAnimation;
    p.intVal0 = m_animation;
    m_ledClient->sendLedGadgetProtocol(p);
}
