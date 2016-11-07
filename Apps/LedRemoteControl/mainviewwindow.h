#ifndef MAINVIEWWINDOW_H
#define MAINVIEWWINDOW_H

#include <QMainWindow>
#include "ledcontroller.h"

namespace Ui {
class mainViewWindow;
}

class mainViewWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainViewWindow(QWidget *parent = 0);
    ~mainViewWindow();

private:
    Ui::mainViewWindow *ui;
};

#endif // MAINVIEWWINDOW_H
