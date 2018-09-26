#ifndef UNIVERSALLISTENER_H
#define UNIVERSALLISTENER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class UniversalListener : public QObject
{
    Q_OBJECT

    QTcpServer _server;
    QTcpSocket *_connection = nullptr;

    bool _isConnected = false;

public:
    explicit UniversalListener(QObject *parent = nullptr);
    ~UniversalListener();

    void initConnections();
    void startListener(int port);

signals:

private slots:
    // --- QTcpServer ---
    void _eventListenerAcceptError(QAbstractSocket::SocketError err);
    void _eventListenerNewConnection();

    // --- QTcpSocket, QAbstractSocket ---
    void _eventSocketConnected();
    void _eventSocketDisconnected();
    void _eventSocketError(QAbstractSocket::SocketError err);
    void _eventSocketStateChanged();

    // --- QTcpSocket, QIODevice ---
    void _eventIODeviceAboutToClose();
    void _eventIODeviceBytesWritten(qint64 bytes);
    void _eventIODeviceReadyRead();
};

#endif // UNIVERSALLISTENER_H
