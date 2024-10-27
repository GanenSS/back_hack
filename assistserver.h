#ifndef ASSISTSERVER_H
#define ASSISTSERVER_H

#include <QCoreApplication>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QThread>

#include "passBD/passdb.h"


class AssistServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit AssistServer(QObject *parent = nullptr);

private:
    passDB DB;

private slots:
    void onNewConnection();
    void processRequest(QTcpSocket *socket);
    //void threadConnection();
};

#endif // ASSISTSERVER_H
