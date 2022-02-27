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



    etp.setTry_find_peer_faild_callback([&]{
        if(!portSet_iter->isEmpty()){
            if(portSet_iter != portSet.end()){
                portSet_iter++;
            }else{

            }
            if(mySerialPort!=nullptr && mySerialPort->isOpen()){
                mySerialPort->close();
            }
            configSerial(mySerialPort,*(portSet_iter));
        }

    });

    etp.bindSendBuffer([&](const std::vector<byte> &senddata)
    {
        if(mySerialPort!=nullptr && mySerialPort->isOpen() && mySerialPort->isWritable()){
            mySerialPort->write(senddata.data(),senddata.size());
            mySerialPort->flush();
            qDebug()<<"SendBuffer:"<<senddata<<endl;
        }
    });


    //    etp.registerCmdCallback(0x05, this, &iotest::test);


    QThread* check_com = QThread::create([&]{
        this->checkAvailablePorts();
        portSet_iter = portSet.begin();
        etp.start();
    });
    check_com->start();

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



void MainWindow::configSerial(QSerialPort *&pserial, const QString &comname)
{
    pserial = new QSerialPort(comname);
    pserial->setBaudRate(QSerialPort::Baud115200);
    pserial->setParity(QSerialPort::NoParity);
    pserial->setDataBits(QSerialPort::Data8);
    pserial->setStopBits(QSerialPort::OneStop);
    pserial->setFlowControl(QSerialPort::NoFlowControl);
    //    pserial->clearError();
    //    pserial->clear();

    connect(mySerialPort, SIGNAL(readyRead()), this, SLOT(readMyCom()));

}


void MainWindow::readMyCom()
{
    QByteArray temp = mySerialPort->readAll();
    qDebug()<<"readMyCom"<<temp<<endl;
    SimpleDPP & sdp = etp.getSimpleDPP();
    sdp.parse(temp.data(),temp.size());
}

