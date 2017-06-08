#ifndef LEDCONTROLERSERVER_H
#define LEDCONTROLERSERVER_H

#include "ledcontroller.h"
#include <QTcpServer>

class ledControlerServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ledControlerServer(QObject *parent = 0);
    QList<ledController*>& nodes() {return m_nodes;}

protected:
    QList<ledController*> m_nodes;

    void addNode(ledController* nNode);

signals:
    void newNode(ledController* nNode);
protected slots:
    void handleNewConn();
public slots:
    void connectNode(QString host, qint16 port);
    void removeNode(QObject *obj);
};

#endif // LEDCONTROLERSERVER_H
