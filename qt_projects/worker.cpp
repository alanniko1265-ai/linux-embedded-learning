#include "worker.h"
#include <QThread>
void Worker::doWork(){
    for(int i=1;i<=10;i++){
        emit progress(QString("后台任务运行中：%1").arg(i));
        QThread::sleep(1);
    }
    emit finished();
}