#include "mainviewwindow.h"
#include "ui_mainviewwindow.h"

mainViewWindow::mainViewWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainViewWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("QGroupBox { border: 1px solid gray; border-radius: 9px; margin-top: 0.5em; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 3px 0 3px;}");
}

mainViewWindow::~mainViewWindow()
{
    delete ui;
}

