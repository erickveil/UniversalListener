#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    _listener.initConnections();

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbStartListener_clicked()
{
    int port = 50504;
    std::function<void (QByteArray)> parseCallback =
            [](QByteArray response) {
        qDebug() << "Parse callback: " << response;

    };
    std::function<QByteArray (QByteArray)> ackCallback =
            [](QByteArray response) {
        qDebug() << "Ack callback: " << response;
        return "Ack";
    };
    std::function<void (QAbstractSocket::SocketError, QString)>  errorCallback =
            [](QAbstractSocket::SocketError errNo, QString errStr) {
        qDebug() << "Error callback: " << QString::number(errNo) << ", " << errStr;
    };

    _listener.initData(port, parseCallback, ackCallback, errorCallback);
    _listener.startListener();
}
