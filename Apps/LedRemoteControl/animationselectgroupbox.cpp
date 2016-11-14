#include "animationselectgroupbox.h"
#include "ui_animationselectgroupbox.h"

animationSelectGroupbox::animationSelectGroupbox(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::animationSelectGroupbox)
{
    ui->setupUi(this);
    m_populated = false;
}

animationSelectGroupbox::~animationSelectGroupbox()
{
    delete ui;
}

void animationSelectGroupbox::setClient(ledController *c)
{
    m_client = c;
    connect(c,SIGNAL(connected(bool))   ,this,SLOT(setEnabled(bool)));
    connect(c,SIGNAL(disconnected(bool)),this,SLOT(setDisabled(bool)));
    connect(c,SIGNAL(ready()),this,SLOT(populate()));
    connect(c,   SIGNAL(animationList(ledGadgetAnimationListPacket)),
            this,SLOT(populate(ledGadgetAnimationListPacket)));
    connect(ui->btnReload,SIGNAL(clicked(bool)),this,SLOT(reload()));
    ui->gbEQ->setClient(c);

}

void animationSelectGroupbox::reload()
{
    for(int i = 0 ; i < m_widgets.count() ; i++)
    {
        m_widgets.first()->deleteLater();
        m_widgets.removeFirst();
    }
    m_populated = false;
    populate();
}

void animationSelectGroupbox::populate()
{
    if(m_populated)
        return;
    m_client->getAnimations();
    m_populated = true;
}

void animationSelectGroupbox::populate(ledGadgetAnimationListPacket p)
{
    for(int i = 0 ; i < p.usedEntries ; i++)
    {
        qDebug() << "Añadiendo animacion : " << QString(p.animationName[i]);
        animationFrame* f = new animationFrame(p.animationID[i],QString(p.animationName[i]),m_client);
        ui->scrollAreaWidgetContents->layout()->addWidget(f);
        m_widgets.append(f);
    }
}
