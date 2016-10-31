#ifndef CONNECTWIDGET_H
#define CONNECTWIDGET_H

#include <QGroupBox>
#include "protocolclient.h"

namespace Ui {
class connectWidget;
}

class connectWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit connectWidget(QWidget *parent = 0);
    ~connectWidget();
    void setClient(protocolClient* c);

private:
    Ui::connectWidget*  ui;
    protocolClient*     m_client;

signals:
    void ready();
    void disconnected();
    void status(bool);

protected slots:
    void clientDisconnected();
    void clientConnected();
    void clientConnecting();
    void clientReady();
    void clientError();
    void connectClient();
    void disconnectClient();
    void socketError(QAbstractSocket::SocketError);
};

#endif // CONNECTWIDGET_H
