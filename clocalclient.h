#ifndef CLOCALCLIENT_H
#define CLOCALCLIENT_H

#include <QObject>

class QLocalSocket;
class CLocalClient : public QObject
{
    Q_OBJECT

signals:
    void signal_readFinished(const QString &msg);

public:
    explicit CLocalClient(QObject *parent = 0);

    bool connectToServer(QLocalSocket *socket, const QString &serverName);
    bool activateWindow(const QString &serverName);
    bool quitProgram(const QString &serverName);

public slots:
    bool slot_sendMsg(const QString &serverName, const QString &msg);
    void slot_readMsg();
};

#endif // CLOCALCLIENT_H
