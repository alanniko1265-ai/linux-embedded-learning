#ifndef LOGMANAGER_H
#define LOGMANAGER_H
#include <QString>
class QTextEdit;
class LogManager{
public:
    explicit LogManager(QTextEdit *logEdit);
    void append(const QString &level,const QString &message);
    void info(const QString &message);
    void error(const QString &message);
    void tcp(const QString &message);
    void send(const QString &message);
    void recive(const QString &message);
    void clear();
private:
    QTextEdit *m_logEdit;
};

#endif // LOGMANAGER_H
