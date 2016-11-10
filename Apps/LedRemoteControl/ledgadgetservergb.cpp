#include "ledgadgetservergb.h"
#include "ui_ledgadgetservergb.h"

ledGadgetServerGB::ledGadgetServerGB(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ledGadgetServerGB)
{
    ui->setupUi(this);
    m_displayedWidget = 0;
    connect(ui->btnAdd ,SIGNAL(clicked(bool)),this,SLOT(addWidget()));
    connect(ui->btnBack,SIGNAL(clicked(bool)),this,SLOT(hideClientWidget()));
    ui->frameClientView->hide();
    connect(&m_server,SIGNAL(newNode(ledController*)),this,SLOT(addClient(ledController*)));
    m_server.listen(QHostAddress::Any,31416);
}

ledGadgetServerGB::~ledGadgetServerGB()
{
    delete ui;
}

void ledGadgetServerGB::addWidget(ledGadgetClientWidget *c)
{
    if(!c)
        c = new ledGadgetClientWidget();
    ui->scrollLedGadgetWidgetContents->layout()->addWidget(c);
    connect(c,SIGNAL(showWidget(ledGadgetGroupBox*)),this,SLOT(showClientWidget(ledGadgetGroupBox*)));
}

void ledGadgetServerGB::showClientWidget(ledGadgetGroupBox* w)
{
    m_displayedWidget = w;
    connect(w,SIGNAL(destroyed(QObject*)),this,SLOT(shownWidgetDestroyed()));
    ui->gbDevices->hide();
    ui->frameClientView->layout()->addWidget(w);
    ui->frameClientView->show();
}

void ledGadgetServerGB::hideClientWidget()
{
    if(m_displayedWidget)
        m_displayedWidget->deleteLater();
    m_displayedWidget = 0;
}

void ledGadgetServerGB::shownWidgetDestroyed()
{
    m_displayedWidget = 0;
    ui->gbDevices->show();
    ui->frameClientView->hide();
}

void ledGadgetServerGB::addClient(ledController *lc)
{
    addWidget(new ledGadgetClientWidget(lc));
}
