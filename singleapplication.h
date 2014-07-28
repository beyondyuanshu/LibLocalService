#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QApplication>

class QLocalServer;
class QWidget;
class SingleApplication : public QApplication
{
    Q_OBJECT

signals:
    void signal_newConnection();

public:
    SingleApplication(int argc, char **argv, bool GUIenabled = true);
    SingleApplication(const QString &id, int &argc, char **argv);

    bool isRunning();

    void setActivationWindow(QWidget *aw, bool activateOnMessage = true);

private slots:    
    void slot_activateWinodw();

private:
    void sysInit(const QString &appId = QString());

    QLocalServer *m_server;
    bool m_isRunning;
    QWidget *m_actWin;
};

#endif // SINGLEAPPLICATION_H
