#include "logmanager.h"
#include <QDatetime>
#include <QTextEdit>
LogManager::LogManager(QTextEdit *logEdit):m_logEdit(logEdit){}
void LogManager::append(const QString &level,const QString &message){
    if(!m_logEdit){
        return;
    }
    QString time=QDateTime::currentDateTime().toString("HH:mm:ss");
    m_logEdit->append("["+time+"]["+level+"]"+message);
}

void LogManager::info(const QString &message){
    append("INFO",message);
}
void LogManager::error(const QString &message){
    append("ERROR",message);
}
void LogManager::recive(const QString &message){
    append("RECIVE",message);
}
void LogManager::send(const QString &message){
    append("SEND",message);
}
void LogManager::tcp(const QString &message){
    append("TCP",message);
}
void LogManager::clear(){
    if(!m_logEdit){
        return;
    }
    m_logEdit->clear();
}