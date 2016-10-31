#ifndef SIMPLEPROTOCOLCOMMANDSGROUPBOX_H
#define SIMPLEPROTOCOLCOMMANDSGROUPBOX_H

#include <QGroupBox>
#include "protocolclient.h"

namespace Ui {
class simpleProtocolCommandsGroupBox;
}

class simpleProtocolCommandsGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit simpleProtocolCommandsGroupBox(QWidget *parent = 0);
    ~simpleProtocolCommandsGroupBox();
    void setProtocolClient(protocolClient *c);


private:
    Ui::simpleProtocolCommandsGroupBox *ui;
    protocolClient*                     m_client;
};

#endif // SIMPLEPROTOCOLCOMMANDSGROUPBOX_H
