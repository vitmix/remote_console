#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "Message.h"
#include "ConsoleCore.h"
#include "CmdProcess.h"



class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    void Start();
    bool Authenticate(QString login, QString password);
public slots:
    void slotNewConnection();
   // void slotServerRead();
    void slotClientDisconnected();

private:
    QTcpServer * tcpServer;
    QTcpSocket * tcpSocket;
    QString login;
    QString password;
};

#endif // SERVER_H
