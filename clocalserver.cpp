#include "clocalserver.h"
#include <QLocalServer>
#include <QLocalSocket>
#include <QByteArray>
#include <QDataStream>
#include <QTimer>
#include "zlog.h"

#ifdef _WIN32
#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#endif

CLocalServer::CLocalServer(QObject *parent) :
    QObject(parent)
  , m_server(0) {
    m_server = new QLocalServer(this);
    connect(m_server, SIGNAL(newConnection()), this, SLOT(slot_dealConnection()));
}

CLocalServer::~CLocalServer() {
    m_server->close();
    delete m_server;
}

bool CLocalServer::listen(const QString &serverName) {
    QLocalSocket ls;
    ls.connectToServer(serverName);
    if (ls.waitForConnected(500)) {
        DBG("Server has run.\n");
        ls.disconnectFromServer();
        ls.close();
        return true;
    }

    m_server->removeServer(serverName);  // NOTE: useful on unix
    if (!m_server->listen(serverName)) {
        DBG("Server listen error: %s\n", m_server->errorString().toLocal8Bit().data());
        return false;
    }

    return true;
}

void CLocalServer::slot_dealConnection() {
    QLocalSocket *socketClient = m_server->nextPendingConnection();
    connect(socketClient, SIGNAL(disconnected()), socketClient, SLOT(deleteLater()));
    connect(socketClient, SIGNAL(readyRead()), this, SLOT(slot_readMsg()));
}

void CLocalServer::slot_sendMsg(QLocalSocket *socket, const QString &msg) {
    QByteArray block;
    QDataStream out(&block, QIODevice::ReadWrite);
    out.setVersion(QDataStream::Qt_4_8);
    out << msg;
    out.device()->seek(0);
    socket->write(block);
    socket->flush();
}

void CLocalServer::slot_readMsg() {
    QLocalSocket *socket = (QLocalSocket *)sender();
    if (socket) {
        if (socket->bytesAvailable() < (int)sizeof(quint16)) {
            socket->waitForReadyRead();
        }

        QDataStream in(socket);
        in.setVersion(QDataStream::Qt_4_8);
        QString msg;
        in >> msg;

        socket->disconnectFromServer();
        if (msg == "ACTIVE_WINDOW") {
            emit signal_activateWindow();
        } else if (msg == "QUIT") {
            emit signal_quitProgram();
            QTimer::singleShot(3000, this, SLOT(slot_forceQuit()));
        }

        emit signal_readFinished(msg);
    }
}

void CLocalServer::slot_forceQuit() {
    DBG("Force quit the program.\n");
    ulong currentPid;
#ifdef _WIN32
    currentPid = GetCurrentProcessId();
    HANDLE currentHandle = OpenProcess(PROCESS_TERMINATE, FALSE, ulong(currentPid));
    if(currentHandle != NULL) {
        TerminateProcess(currentHandle, 0);
    }
#else
    currentPid = getpid();
    kill(currentPid, SIGKILL);
#endif
}
