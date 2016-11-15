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
    ui->labelName->setText(m_animationName);

    ui->gbIntVal0->hide();
    ui->gbIntVal1->hide();
    ui->gbIntVal2->hide();
    ui->gbIntVal3->hide();
    ui->gbFloatVal0->hide();
    ui->gbFloatVal1->hide();
    ui->gbFloatVal2->hide();
    ui->gbFloatVal3->hide();

    connect(ui->btnActivate,SIGNAL(clicked(bool)),this,SLOT(setAnimation()));

    if      (m_animation == animationStrobe)
    {
        ui->gbIntVal0->setTitle("Hz");
        ui->gbIntVal0->show();
    }
}

void animationFrame::setAnimation()
{
    ledGadgetProtocolPacket p;
    p.cmd = cmdSetAnimation;
    p.intVal0 = m_animation;
    m_ledClient->sendLedGadgetProtocol(p);
}
