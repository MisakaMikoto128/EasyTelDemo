#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <iostream>
#include <cstring>
using namespace std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QThread* check_com = QThread::create([&]{
        this->checkAvailablePorts();
    });
    etp.bindSendBuffer([&](const std::vector<byte> &senddata)
     {


    });


//    etp.registerCmdCallback(0x05, this, &iotest::test);
    etp.start();
}

MainWindow::~MainWindow()
{
    etp.stop();
    delete ui;
}
void MainWindow::checkAvailablePorts(){
    foreach ( const QSerialPortInfo &Info, QSerialPortInfo::availablePorts()) {
        QSerialPort availablePort;
        availablePort.setPortName(Info.portName());
        if (availablePort.open(QIODevice::ReadWrite)) {
            portSet.insert(Info.portName());
            qDebug()<<Info.portName()<<endl;
            availablePort.close();
        }else{
            qDebug()<<"can't open:"<<Info.portName()<<endl;
        }
    }
}


