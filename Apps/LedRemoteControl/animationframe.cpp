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
    ui->editText->hide();
    connect(ui->btnActivate,SIGNAL(clicked(bool)),this,SLOT(setAnimation()));

    if      (m_animation == animationStrobe)
    {
        ui->gbIntVal0->setTitle("Hz");
        ui->gbIntVal0->show();
        ui->spinInt0->setMinimum(1);
        ui->spinInt0->setMaximum(100);
        ui->sliderInt0->setMinimum(1);
        ui->sliderInt0->setMaximum(100);
        this->setMinimumHeight(160);
    } else if(m_animation == animationScrollText)
    {
        ui->editText->show();
    }
}

void animationFrame::setAnimation()
{
    ledGadgetProtocolPacket p;
    p.cmd = cmdSetAnimation;
    p.intVal0 = m_animation;
    if(m_animation == animationScrollText)
        for(int i = 0 ; i < ui->editText->text().length() ; i++)
            p.stringVal[i] = ui->editText->text().toUtf8().at(i);
    else if(m_animation == animationStrobe)
        p.intVal1 = ui->spinInt0->value();
    m_ledClient->sendLedGadgetProtocol(p);
}
