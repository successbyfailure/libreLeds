#include "mainviewwindow.h"
#include "ui_mainviewwindow.h"

mainViewWindow::mainViewWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainViewWindow)
{
    ui->setupUi(this);
}

mainViewWindow::~mainViewWindow()
{
    delete ui;
}

