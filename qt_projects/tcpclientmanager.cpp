#include "tcpclientmanager.h"
TcpClientManager::TcpClientManager(QObject *parent):QObject(parent),m_socket(new QTcpSocket(this)){
    connect(m_socket,&QTcpSocket::connected,this,[this](){
        emit connected();
    });
    connect(m_socket,&QTcpSocket::disconnected,this,[this](){
        emit disconnected();
    });
    connect(m_socket,&QTcpSocket::readyRead,this,[this](){
        QByteArray date=m_socket->readAll();
        QString text=QString::fromUtf8(date);
        emit dateReceived(text);
    });
    connect(m_socket,&QTcpSocket::errorOccurred,this,[this](){
        emit errorOccurred(m_socket->errorString());
    });
}
void TcpClientManager::connectToHost(const QString &ip,int port){
    m_socket->connectToHost(ip,port);
}
void TcpClientManager::disconnectFromHost(){
    m_socket->disconnectFromHost();
}
void TcpClientManager::sendText(const QString &text){
    QByteArray date=text.toUtf8();
    qint64 bytes=m_socket->write(date);
    if(bytes==-1){
        emit errorOccurred(m_socket->errorString());
        return;
    }
    emit dateSent(text,bytes);
}
bool TcpClientManager::isConnected()const{
    return m_socket->state()==QAbstractSocket::ConnectedState;
}