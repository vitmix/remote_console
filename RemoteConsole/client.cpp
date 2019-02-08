#include "client.h"

client::client(QObject *parent) : QObject(parent)
{

}

void client::Connect()
{
    socket = new QTcpSocket(this);
    socket->connectToHost("127.0.0.1", 1234);

    if(socket->waitForConnected(3000))
    {
        qDebug() << "Connected!";
        socket->waitForReadyRead(3000);
        QString socketText(socket->readAll());
        qDebug() << "MyClient::readyRead()";
        qDebug() <<"Socket text: " << socketText;
       // connect(socket, &QTcpSocket::readyRead, this, &client::slotClientRead);
        QByteArray qbLogin;
        QTextStream qtin(stdin);
        qDebug()<<"Login:";
        QString myLogin = qtin.readLine();
        qbLogin.append(myLogin);
        socket->write(qbLogin);
        socket->waitForBytesWritten(3000);
        QByteArray qbPass;
        qDebug()<<"Password:";
        QString myPass = qtin.readLine();
        qbPass.append(myPass);
        socket->write(qbPass);
        socket->waitForBytesWritten(3000);
        socket->waitForReadyRead(-1);
        QString authAnswer(socket->readAll());
        qDebug()<< authAnswer;
        ConsoleCore cconsole;
        cconsole.SetConsoleSize({120,9000});

        Message<INPUT_RECORD> irpackage;
        Message<CHAR_INFO> cipackage;
        Message<COORD> coordpackage;
        std::vector<COORD> coords(1);

        while (true) {
          cconsole.UpdateReadingInfo();

          coords[0] = cconsole.GetBufferSize();
          coordpackage.SetCommand("BuffSize");
          coordpackage.SetData(coords);
          std::string strSerial = Serialize(coordpackage);
          QByteArray Buffer(strSerial.c_str(), strSerial.size());
          socket->write(Buffer);
          socket->waitForBytesWritten(-1);
          socket->waitForReadyRead(-1);

           QByteArray Buffer2Size(socket->readAll());

           long int sizeBuff = Buffer2Size.toInt();
           QByteArray Buffer2;

           cipackage.SetCommand("Submit");
           cipackage.GetData().clear();
           std::string strSubmitSerial1 = Serialize(cipackage);
            QByteArray BufferSubmit1(strSubmitSerial1.c_str(), strSubmitSerial1.size());
            socket->write(BufferSubmit1);
            socket->waitForBytesWritten(-1);

            while(Buffer2.length()!=sizeBuff)
            {
                socket->waitForReadyRead(3000);
                QByteArray Buffer21(socket->readAll());
                Buffer2.append(Buffer21);
                cipackage.SetCommand("Submit");
                cipackage.GetData().clear();
                std::string strSubmitSerial2 = Serialize(cipackage);
                QByteArray BufferSubmit2(strSubmitSerial2.c_str(), strSubmitSerial2.size());
               // qDebug()<<"Submit Size : "<<BufferSubmit.size();
                socket->write(BufferSubmit2);
                socket->waitForBytesWritten(-1);
            }
            std::string strDeserial(Buffer2.constData(),Buffer2.length());

          Deserialize(cipackage, strDeserial);
          if(cipackage.GetCommand()=="OutputBuff"){
              cconsole.WriteOutputToConsole(cipackage.GetData());
          }

          cipackage.SetCommand("Submit");
          cipackage.GetData().clear();
          std::string strSubmitSerial3 = Serialize(cipackage);
          QByteArray BufferSubmit3(strSubmitSerial3.c_str(), strSubmitSerial3.size());
          socket->write(BufferSubmit3);
          socket->waitForBytesWritten(-1);

          socket->waitForReadyRead(-1);

          QByteArray Buffer3(socket->readAll());
          std::string strDeserial2(Buffer3.constData(),Buffer3.length());
          Deserialize(coordpackage, strDeserial2);
          if(coordpackage.GetCommand()=="CursorPos"){
              cconsole.SetCursorPos(coordpackage.GetData()[0]);
          }

          std::vector<INPUT_RECORD> irBuff = cconsole.ReadInputFromConsole();
          irpackage.SetCommand("InputBuff");
          irpackage.SetData(irBuff);
          std::string strSerial2 = Serialize(irpackage);
          QByteArray Buffer4(strSerial2.c_str(), strSerial2.size());
          socket->write(Buffer4);

          socket->waitForBytesWritten(-1);

          socket->waitForReadyRead(-1);
          QByteArray BuffSubmit(socket->readAll());
          std::string strDeserialSubmit(BuffSubmit.constData(), BuffSubmit.length());
          Deserialize(cipackage, strDeserialSubmit);
          if(cipackage.GetCommand()!="Submit"){
              break;
          }
        }
    }    else
    {
        qDebug() << "Not connected!";
    }
}

/*void client::slotClientRead()
  {
      QString socketText(socket->readAll());
      qDebug() << "MyClient::readyRead()";
      qDebug() <<"Socket text: " << socketText;
      QTextStream qtin(stdin);
      QString command = qtin.readLine();
      QByteArray Buffer;
      Buffer.append(command);
      socket->write(Buffer);
  }*/
