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

    etp.bindSendBuffer(this,&MainWindow::SendBuffer);


    etp.setFind_peer_iteration_callback([&]{
        bool have = false;
        if(ioAdapter->hasNext()){
            mySerialPort = ioAdapter->next();
            have = true;
        }
        return have;
    });



    QThread* check_com = QThread::create([&]{
        //        this->checkAvailablePorts();
        portSet.insert("COM1");
    });
    check_com->start();
    connect(check_com,SIGNAL(finished()),this,SLOT(initFindPeer()));

    QThread* debug_isFound = QThread::create([&]{
        while(!etp.isFoundPoint()){}

        qDebug()<<"FoundPoint"<<mySerialPort->portName()<<endl;

        for(auto i = serialSet.begin();i!=serialSet.end();i++){
            if(*i != mySerialPort){
                (*i)->close();
            }
        }
    });
    debug_isFound->start();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete ioAdapter;
    ioAdapter = nullptr;
    for(auto i = serialSet.begin();i!=serialSet.end();i++){
        delete (*i);
    }
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



void MainWindow::configSerial(QSerialPort *&pserial)
{
    pserial->setBaudRate(QSerialPort::Baud115200);
    pserial->setParity(QSerialPort::NoParity);
    pserial->setDataBits(QSerialPort::Data8);
    pserial->setStopBits(QSerialPort::OneStop);
    pserial->setFlowControl(QSerialPort::NoFlowControl);
    //    pserial->clearError();
    //    pserial->clear();
    connect(pserial, SIGNAL(readyRead()), this, SLOT(readMyCom()));
}


void MainWindow::readMyCom()
{
    QByteArray temp = mySerialPort->readAll();
    qDebug()<<"REV:"<<temp<<endl;
    etp.parse(temp.data(),temp.size());
}

void MainWindow::initFindPeer()
{
    QSetIterator<QString> *iter = new QSetIterator<QString>(portSet);

    while(iter->hasNext()){
        QSerialPort *serialPort = new QSerialPort(iter->next());
        configSerial(serialPort);
        serialSet.insert(serialPort);

        if (serialPort->open(QIODevice::ReadWrite)) {

        }
    }
    ioAdapter = new QSetIterator<QSerialPort*>(serialSet);
    etp.start();

}

void MainWindow::SendBuffer(const QByteArray &senddata)
{

    qDebug()<<"send data:"<<senddata<<endl;

    QSerialPort *port = mySerialPort;
    if(port!=nullptr && port->isOpen() && port->isWritable()){
        port->write(senddata);
        port->flush();
        port->waitForBytesWritten(500);
    }
}

