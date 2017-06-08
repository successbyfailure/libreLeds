#include "ledcontrolerserver.h"

ledControlerServer::ledControlerServer(QObject *parent) :
    QTcpServer(parent)
{
    connect(this,SIGNAL(newConnection()),this,SLOT(handleNewConn()));
}

void ledControlerServer::handleNewConn()
{
    qDebug() << "[D]sJarvisNodeServer::validateClient-> New client!";
    ledController* n = new ledController(this->nextPendingConnection());
    addNode(n);
}

void ledControlerServer::addNode(ledController *nNode)
{
    connect(nNode,SIGNAL(disconnected())     ,nNode,SLOT(deleteLater()));
    connect(nNode,SIGNAL(destroyed(QObject*)),this  ,SLOT(removeNode(QObject*)));

    m_nodes.append(nNode);
    emit newNode(nNode);
}

void ledControlerServer::connectNode(QString host, qint16 port)
{
    ledController* l = new ledController();
    addNode(l);
    l->connectToHost(host,port);
}

void ledControlerServer::removeNode(QObject *obj)
{
    m_nodes.removeAll((ledController*)obj);
}

