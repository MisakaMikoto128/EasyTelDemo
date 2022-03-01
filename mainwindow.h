#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "EasyTel.hpp"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QSet>
#include <QString>
#include <QTimer>
#include <QThread>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void readMyCom();
    void initFindPeer();
    void SendBuffer(const QByteArray &senddata);
signals:
    void writeMyComS();
private:
    Ui::MainWindow *ui;
    EasyTelPoint etp;

    QSerialPort *mySerialPort = nullptr;
    QSet<QString> portSet;
    QSet<QSerialPort*> serialSet;
    QSetIterator<QSerialPort*> *ioAdapter;
    void configSerial(QSerialPort * &pserial);
    void checkAvailablePorts();
};
#endif // MAINWINDOW_H
