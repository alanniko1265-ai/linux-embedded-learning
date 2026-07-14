#ifndef TCPCLIENTMANAGER_H
#define TCPCLIENTMANAGER_H
#include <QObject>
#include <QTcpSocket>
class TcpClientManager : public QObject
{
    Q_OBJECT
public:
    explicit TcpClientManager(QObject *parent =nullptr);
    void connectToHost(const QString &ip,int port);
    void disconnectFromHost();
    void sendText(const QString &text);
    bool isConnected() const;
signals:
    void connected();
    void disconnected();
    void dateReceived(const QString &text);
    void errorOccurred(const QString &message);
    void dateSent(const QString &text,qint64 bytes);
private:
    QTcpSocket *m_socket;
};

#endif // TCPCLIENTMANAGER_H
