#ifndef CLOCALSERVER_H
#define CLOCALSERVER_H

#include <QObject>

class QLocalServer;
class QLocalSocket;
class CLocalServer : public QObject
{
    Q_OBJECT

    QLocalServer *m_server;

signals:
    void signal_activateWindow();
    void signal_quitProgram();
    void signal_readFinished(const QString &msg);

public:
    explicit CLocalServer(QObject *parent = 0);
    ~CLocalServer();

    bool listen(const QString &serverName);

public slots:
    void slot_dealConnection();
    void slot_sendMsg(QLocalSocket *socket, const QString &msg);
    void slot_readMsg();

private slots:
    void slot_forceQuit();
};

#endif // CLOCALSERVER_H
