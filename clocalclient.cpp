#include "clocalclient.h"
#include <QLocalSocket>
#include <QDataStream>
#include <QByteArray>
#include "zlog.h"

CLocalClient::CLocalClient(QObject *parent) :
    QObject(parent)
{
}

bool CLocalClient::connectToServer(QLocalSocket *socket, const QString &serverName) {
    bool ret = false;
    socket->abort();
    socket->connectToServer(serverName);
    if (socket->waitForConnected(500)) {
        ret = true;
    }
    return ret;
}

bool CLocalClient::slot_sendMsg(const QString &serverName, const QString &msg) {
    QLocalSocket *socket = new QLocalSocket;
    connect(socket, SIGNAL(readyRead()), this, SLOT(slot_readMsg()));
    if (connectToServer(socket, serverName)) {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << msg;
        out.device()->seek(0);
        socket->write(block);
        socket->flush();
        if (socket->waitForReadyRead(3000)) {
            DBG("Send message ready.\n");
            return true;
        }
    } else {
        delete socket;
    }
    return false;
}

void CLocalClient::slot_readMsg() {
    QLocalSocket *socket = (QLocalSocket *)sender();
    if (!socket)
        return;

    if (socket->bytesAvailable() < (int)sizeof(quint16)) {
        socket->waitForReadyRead();
    }

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_8);
    QString msg;
    in >> msg;

    emit signal_readFinished(msg);
}

bool CLocalClient::activateWindow(const QString &serverName) {
    return slot_sendMsg(serverName, "ACTIVE_WINDOW");
}

bool CLocalClient::quitProgram(const QString &serverName) {
    return slot_sendMsg(serverName, "QUIT");
}
