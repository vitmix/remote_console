#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
}

void Server::Start()
{

    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::slotNewConnection);

    if(tcpServer->listen(QHostAddress::Any, 1234)){
        qDebug() << "server is started";
        qDebug()<<"Please right login for user:";
        QTextStream qtin(stdin);
        QString myLogin = qtin.readLine();
        qDebug()<<"Please right password for user:";
        QString myPass = qtin.readLine();
        login = myLogin;
        password = myPass;
    } else {
        qDebug() << "server is not started";
    }
}

void Server::slotNewConnection()
{
    tcpSocket = tcpServer->nextPendingConnection();
    tcpSocket->write("Hello, World!!! I am remote console server!\r\n");
    //connect(tcpSocket, &QTcpSocket::readyRead, this, &Server::slotServerRead);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &Server::slotClientDisconnected);
    tcpSocket->waitForReadyRead(-1);
    QString clientLogin(tcpSocket->readAll());
    tcpSocket->waitForReadyRead(-1);
    QString clientPass(tcpSocket->readAll());
    if(clientLogin == login && clientPass == password)
    {
        QString answer = "Well done";
        QByteArray qbAnswer;
        qbAnswer.append(answer);
        tcpSocket->write(qbAnswer);
        tcpSocket->waitForBytesWritten(3000);
    }else{
        tcpSocket->close();
    }

    CmdProcess cmdProc;
    ConsoleCore sconsole;
    sconsole.UpdateHandles();

    sconsole.SetConsoleSize({120,9000});

    Message<INPUT_RECORD> irpackage;
    Message<CHAR_INFO> cipackage;
    Message<COORD> coordpackage;
    std::vector<COORD> coords(1);


    while (true) {
        tcpSocket->waitForReadyRead(-1);
        QByteArray Buff1(tcpSocket->readAll());
        std::string strDeserial(Buff1.constData(), Buff1.length());
        Deserialize(coordpackage, strDeserial);
        if(coordpackage.GetCommand()=="BuffSize"){
            sconsole.SetConsoleSize(coordpackage.GetData()[0]);
        }
        sconsole.UpdateReadingInfo();
        std::vector<CHAR_INFO> outBuff(sconsole.ReadOutputFromConsole());
        cipackage.SetCommand("OutputBuff");
        cipackage.SetData(outBuff);
        std::string strSerial = Serialize(cipackage);
        QByteArray Buffer2(strSerial.c_str(),strSerial.size());
        long int sizeBuff = Buffer2.size();


        QByteArray Buffer2Size;
        Buffer2Size.setNum(Buffer2.size());
        tcpSocket->write(Buffer2Size);
        tcpSocket->waitForBytesWritten(-1);

        tcpSocket->waitForReadyRead(-1);
        QByteArray BuffSubmit1(tcpSocket->readAll());
        std::string strDeserialSubmit1(BuffSubmit1.constData(), BuffSubmit1.length());
        Deserialize(cipackage, strDeserialSubmit1);
        if(cipackage.GetCommand()!="Submit"){
            break;
        }

        int checksum=0;
        while(sizeBuff>0){
            QByteArray Buffer2tmp;
           if(sizeBuff<10000)
           {
             QByteArray Buffer2tmp2(&strSerial[checksum],sizeBuff);
             Buffer2tmp.append(Buffer2tmp2);
           }else{
             QByteArray Buffer2tmp2(&strSerial[checksum],10000);
              Buffer2tmp.append(Buffer2tmp2);
           }
        tcpSocket->write(Buffer2tmp);
        tcpSocket->waitForBytesWritten(-1);
        tcpSocket->waitForReadyRead(-1);
        QByteArray BuffSubmit2(tcpSocket->readAll());
        std::string strDeserialSubmit2(BuffSubmit2.constData(), BuffSubmit2.length());
        //qDebug()<< "Submit size:"<<BuffSubmit.size();
        Deserialize(cipackage, strDeserialSubmit2);
        if(cipackage.GetCommand()!="Submit"){
            qDebug()<< "Vse huevo";
            break;
        }
        sizeBuff -= 10000;
        checksum+=10000;
        }
        tcpSocket->waitForReadyRead(-1);
        QByteArray BuffSubmit3(tcpSocket->readAll());
        std::string strDeserialSubmit3(BuffSubmit3.constData(), BuffSubmit3.length());
        Deserialize(cipackage, strDeserialSubmit3);
        if(cipackage.GetCommand()!="Submit"){
            break;
        }
        sconsole.UpdateReadingInfo();
        coords[0] = sconsole.GetCursorPosition();
        coordpackage.SetCommand("CursorPos");
        coordpackage.SetData(coords);

        std::string strSerial2 = Serialize(coordpackage);
        QByteArray Buffer3(strSerial2.c_str(),strSerial2.size());
        tcpSocket->write(Buffer3);

        tcpSocket->waitForBytesWritten(-1);
        //qDebug()<<"Vladik saset pipirku3";
        tcpSocket->waitForReadyRead(-1);
       // qDebug()<<"Vladik saset pipierku4";

        QByteArray Buffer4(tcpSocket->readAll());
        std::string strDeserial2(Buffer4.constData(),Buffer4.length());
        Deserialize(irpackage, strDeserial2);
        if(irpackage.GetCommand()=="InputBuff"){
            sconsole.WriteInputToConsole(irpackage.GetData());
        }

        cipackage.SetCommand("Submit");
        cipackage.GetData().clear();
        std::string strSubmitSerial = Serialize(cipackage);
        QByteArray BufferSubmit(strSubmitSerial.c_str(), strSubmitSerial.size());
       // qDebug()<<"Submit Size : "<<BufferSubmit.size();
        tcpSocket->write(BufferSubmit);
        tcpSocket->waitForBytesWritten(-1);
    }
}

/*void Server::slotServerRead()
{
    while(tcpSocket->bytesAvailable()>0)
    {
        QByteArray array = tcpSocket->readAll();

        tcpSocket->write(array);
   }
}*/

void Server::slotClientDisconnected()
{
    tcpSocket->close();
}
