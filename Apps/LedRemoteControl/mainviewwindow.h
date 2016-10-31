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
    void setClient(ledController* client, bool deleteObj = true);

private:
    Ui::mainViewWindow *ui;
    ledController*      m_ledClient;
    bool                m_deleteClient;
};

#endif // MAINVIEWWINDOW_H
