#include "singleapplication.h"
#include <QLocalServer>
#include <QLocalSocket>
#include <QByteArray>
#include <QFile>
#include <QWidget>
#include "zlog.h"

SingleApplication::SingleApplication(int argc,char **argv, bool GUIenabled) :
    QApplication(argc, argv, GUIenabled)
  , m_isRunning(false)
  , m_server(0)
  , m_actWin(0) {
    sysInit();
}

SingleApplication::SingleApplication(const QString &id, int &argc, char **argv) :
    QApplication(argc, argv)
  , m_isRunning(false)
  , m_server(0)
  , m_actWin(0) {
    sysInit(id);
}

void SingleApplication::sysInit(const QString &appId) {
    QString id = appId;
    QString prefix = id;
    if (id.isEmpty()) {
        id = QCoreApplication::applicationFilePath();
#if defined(Q_OS_WIN)
        id = id.toLower();
#endif
        prefix = id.section(QLatin1Char('/'), -1);
    }
    prefix.remove(QRegExp("[^a-zA-Z]"));
    prefix.truncate(6);

    QByteArray idc = id.toUtf8();
    quint16 idNum = qChecksum(idc.constData(), idc.size());
    QString serverName = QLatin1String("qtsingleapp-") + prefix
            + QLatin1Char('-') + QString::number(idNum, 16);

    QLocalSocket socket;
    socket.connectToServer(serverName);

    if(socket.waitForConnected(500)) {
        DBG("There is already a instance running.\n");
        m_isRunning = true;
        return;
    }

    DBG("Can't connect to the server, build one.\n");
    m_server = new QLocalServer(this);
    connect(m_server, SIGNAL(newConnection()), this, SIGNAL(signal_newConnection()));
    m_server->removeServer(serverName);  // NOTE: useful on unix
    if (!m_server->listen(serverName)) {
        DBG("Server listen error: %s\n", m_server->errorString().toLocal8Bit().data());
    }
}

void SingleApplication::setActivationWindow(QWidget *aw, bool activateOnMessage) {
    m_actWin = aw;
    if (activateOnMessage)
        connect(m_server, SIGNAL(newConnection()), this, SLOT(slot_activateWinodw()));
    else
        disconnect(m_server, SIGNAL(newConnection()), this, SLOT(slot_activateWinodw()));
}

void SingleApplication::slot_activateWinodw() {
    DBG("There is already a instance running, raising it up.\n");
    if (m_actWin) {
        m_actWin->setWindowState(m_actWin->windowState() & ~Qt::WindowMinimized);
        m_actWin->raise();
        m_actWin->activateWindow();
    }
}

bool SingleApplication::isRunning() {
    return m_isRunning;
}
