#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include "ConsoleCore.h"
#include "Message.h"

class client : public QObject
{
    Q_OBJECT
public:
    explicit client(QObject *parent = nullptr);
    void Connect();


public slots:

    //void slotClientRead();

private:
    QTcpSocket *socket;
};

#endif // CLIENT_H
