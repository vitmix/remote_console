#include <QCoreApplication>
#include "QDebug"
#include "server.h"
#include "client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int tries = 0;
    Server myServer;
    client myClient;
    while(tries<5)
    {
        tries++;
        qDebug()<<"Choose s - server, c - client";
        QTextStream qtin(stdin);
        QString type = qtin.readLine();
        if(type=="s"){
           myServer.Start();
           break;
        }else if(type=="c"){
            myClient.Connect();
            break;
        }else{
          qDebug()<<"Invalid input, try again";
        }
    }
    return a.exec();
}
