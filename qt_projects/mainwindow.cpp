#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include "QStatusBar"
#include "QTableWidgetItem"
#include "QHeaderView"
#include "QBrush"
#include "QDoubleValidator"
#include "QDateTime"
#include "Qsettings"
#include "worker.h"
#include "logmanager.h"
#include "QFileDialog"
#include "QFile"
#include "QTextStream"
#include "tcpclientmanager.h"
#include "settingsmanager.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    logManager =new LogManager(ui->textEditLog);
    this->setStyleSheet(
        "QMainWindow { background-color: white; color: black; }"
        "QWidget { background-color: white; color: black; }"
        "QPushButton:disabled { color: gray; background-color: #eeeeee; }"
        );
    setWindowTitle("DeviceMonitorHost");
    resize(800,600);
    ui->labelStatus->setText("TCP状态：未连接");
    ui->labelStatus->setStyleSheet("color: red;");
    statusBar()->showMessage("TCP未连接");
    ui->comboBoxBaudRate->addItem("9600");
    ui->comboBoxBaudRate->addItem("19200");
    ui->comboBoxBaudRate->addItem("38400");
    ui->comboBoxBaudRate->addItem("57600");
    ui->comboBoxBaudRate->addItem("115200");
    ui->comboBoxBaudRate->setCurrentText("115200");
    ui->textEditSerialReceive->setReadOnly(true);
    initTcpSocket();
    loadSetting();
    scanSerialPorts();
    //startWorkerTask();
    appendLog("DeviceMonitorHost started");
    updateTcpUiState(false);
}
MainWindow::~MainWindow()
{
    saveSetting();
    delete ui;
}
void MainWindow::on_btnConnect_clicked(){
    QString ip=ui->lineEditIp->text().trimmed();
    QString portText=ui->lineEditPort->text().trimmed();
    QString devicename=ui->lineEditDeviceName->text().trimmed();
    if(devicename.isEmpty()){
        QMessageBox::warning(this,"提示","请输入设备名");
        return;
    }
    if(ip.isEmpty()){
        QMessageBox::warning(this,"提示","请输入IP.");
        logManager->error("连接失败: IP为空");
        statusBar()->showMessage("连接失败: IP为空");
        return;
    }
    if(portText.isEmpty()){
        QMessageBox::warning(this,"提示","请输入端口");
        logManager->error("连接失败：端口为空");
        statusBar()->showMessage("连接失败：端口为空");
        return;
    }
    bool ok=false;
    int port=portText.toInt(&ok);
    if(!ok){
        QMessageBox::warning(this,"提示","端口必须是数字.");
        logManager->error("连接失败：端口不是数字");
        statusBar()->showMessage("连接失败：端口不是数字");
        return;
    }
    if(port<1||port>65535){
        QMessageBox::warning(this,"提示","端口范围必须是 1-65535");
        logManager->error("连接失败：端口超出范围");
        statusBar()->showMessage("连接失败：端口超出范围");
        return;
    }
    tcpManager->connectToHost(ip,port);
    logManager->tcp("正在连接到"+ip+":"+QString::number(port));
    statusBar()->showMessage("正在连接到......");
    ui->labelStatus->setText("TCP状态：连接中");
    ui->labelStatus->setStyleSheet("color: orange");
}
void MainWindow::on_btnDisconnect_clicked(){
    if(tcpManager->isConnected()){
        logManager->tcp("用户请求断开TCP连接");
        tcpManager->disconnectFromHost();
    }
    else{
        logManager->info("当前没有已建立的TCP连接");
        ui->labelStatus->setStyleSheet("color: red");
        ui->labelStatus->setText("TCP状态：未连接");
        updateTcpUiState(false);
    }
}
void MainWindow::on_btnClearLog_clicked(){
    int ret=QMessageBox::question(
            this,
            "确认",
            "确认要清空日志吗?"
        );
    if(ret==QMessageBox::Yes){
        logManager->clear();
        statusBar()->showMessage("已经清空日志");
    }
}
void MainWindow::initTcpSocket(){
    tcpManager=new TcpClientManager(this);
    connect(tcpManager,&TcpClientManager::connected,this,[this](){
        logManager->tcp("connected：TCP连接成功");
        ui->labelStatus->setText("TCP状态：已连接");
        ui->labelStatus->setStyleSheet("color:green;");
        statusBar()->showMessage("TCP已连接");
        updateTcpUiState(true);

    });
    connect(tcpManager,&TcpClientManager::disconnected,this,[this](){
        if(ui->labelStatus->text()=="TCP状态：连接错误"){
            logManager->tcp("disconnected：连接失败后 socket 已关闭");
            statusBar()->showMessage("TCP连接失败，socket已关闭");
        }
        else{
            logManager->tcp("disconnected：TCP连接已断开");
            ui->labelStatus->setStyleSheet("color:red");
            ui->labelStatus->setText("TCP状态：未连接");
            statusBar()->showMessage("TCP已断开");
        }
        updateTcpUiState(false);
    });
    connect(tcpManager,&TcpClientManager::dateReceived,this,[this](const QString &text){

        logManager->recive("readyRead:收到数据:"+text);
        if(text.contains("STATUS=OK")){
            logManager->info("设备状态：正常");
            statusBar()->showMessage("设备状态：正常");
        }
        else if(text.contains("ERROR")){
            logManager->error("设备返回异常:"+text);
            statusBar()->showMessage("设备返回异常");
        }
    } );
    connect(tcpManager,&TcpClientManager::errorOccurred,this,[this](const QString &message){
        logManager->error("TCP错误:"+message);
        ui->labelStatus->setText("TCP状态：连接错误");
        ui->labelStatus->setStyleSheet("color:red");
        statusBar()->showMessage("TCP状态：连接错误 "+message);
        updateTcpUiState(false);
    }


            );
    connect(tcpManager,&TcpClientManager::dateSent,this,[this](const QString &text,qint64 bytes){
        logManager->send("TCP发送数据:"+text+",字节数:"+QString::number(bytes));
        statusBar()->showMessage("发送成功");
    });
}
void MainWindow::appendLog(const QString &message){
    logManager->info(message);
}
void MainWindow::scanSerialPorts()
{
    const QList<QSerialPortInfo> ports=QSerialPortInfo::availablePorts();
    appendLog("开始扫描端口");
    for(const QSerialPortInfo &info:ports){
        appendLog("发现串口:1"+info.portName()+"-"+info.description());
    }
    if(ports.isEmpty()){
        appendLog("未发现可用串口");
    }



}
void MainWindow::on_btnSendTcp_clicked(){
    if(!tcpManager->isConnected()){
        logManager->error("TCP发送失败：当前未连接");
        statusBar()->showMessage("TCP发送失败：当前未连接");
        return;
    }
    QString text=ui->lineEditTcpSend->text().trimmed();
    if(text.isEmpty()){
        logManager->error("TCP发送失败：发送内容为空");
        statusBar()->showMessage("TCP发送失败：发送内容为空");
        return;
    }
    tcpManager->sendText(text);
    ui->lineEditTcpSend->clear();
}
void MainWindow::updateTcpUiState(bool connected){
    ui->btnConnect->setEnabled(!connected);
    ui->btnDisconnect->setEnabled(connected);
    ui->btnSendTcp->setEnabled(connected);
}
void MainWindow::loadSetting(){
    AppSettings settings=SettingsManager::load();
    ui->lineEditIp->setText(settings.tcpIp);
    ui->lineEditPort->setText(settings.tcpPort);
    ui->comboBoxBaudRate->setCurrentText(settings.serialBaudRate);
    appendLog("已加载上次配置");
}
void MainWindow::saveSetting(){
    AppSettings settings;
    settings.tcpIp=ui->lineEditIp->text().trimmed();
    settings.tcpPort=ui->lineEditPort->text().trimmed();
    settings.serialBaudRate=ui->comboBoxBaudRate->currentText().trimmed();
    SettingsManager::save(settings);
    appendLog("已保存当前配置");
}
void MainWindow::startWorkerTask(){
    workerThread=new QThread(this);
    Worker *worker=new Worker;
    worker->moveToThread(workerThread);
    connect(workerThread,&QThread::started,worker,&Worker::doWork);
    connect(worker,&Worker::progress,this,[this](const QString message){
        appendLog(message);
    });
    connect(worker,&Worker::finished,workerThread,&QThread::quit);
    connect(worker,&Worker::finished,worker,&Worker::deleteLater);
    connect(workerThread,&QThread::finished,workerThread,&QThread::deleteLater);
    workerThread->start();

}
void MainWindow::on_btnSaveLog_clicked(){
    QString filename=QFileDialog::getSaveFileName(
        this,
        "保存日志",
        "",
        "Text Files (*,txt);;All Files (*,*)"
    );
    if(filename.isEmpty()){
        return;
    }
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly| QIODevice::Text)){
        QMessageBox::warning(this,"错误","日志文件保存失败");
        return;
    }
    QTextStream out(&file);
    out<< ui->textEditLog->toPlainText();
    file.close();
    statusBar()->showMessage("日志已经保存");
}
void MainWindow:: on_btnAbout_triggered(){
    QMessageBox::about(
        this,
        "关于 DeviceMonitorHost",
        "DeviceMonitorHost\n\n"
        "基于 Qt/C++ 的设备通信监控上位机。\n"
        "当前版本支持 TCP 连接、数据发送接收、运行日志、日志保存、错误反馈和参数持久化。\n\n"
        "Serial Port 模块作为后续扩展预留。"
        );
}