#ifndef UNIVERSALLISTENER_H
#define UNIVERSALLISTENER_H

#include <functional>

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class UniversalListener : public QObject
{
    Q_OBJECT

    QTcpServer _server;
    QTcpSocket *_connection = nullptr;

    bool _isConnected = false;
    int _port = -1;

    std::function<void (QByteArray)> _parseCallback;
    std::function<QByteArray (QByteArray)> _ackCallback;
    std::function<void (QAbstractSocket::SocketError, QString)> _errorCallback;

public:
    explicit UniversalListener(QObject *parent = nullptr);
    UniversalListener(int port);
    UniversalListener(int port, std::function<void (QByteArray)> parseCallback);
    UniversalListener(int port,
                      std::function<void (QByteArray)> parseCallback,
                      std::function<QByteArray (QByteArray)> ackCallback );
    UniversalListener(int port,
                      std::function<void (QByteArray)> parseCallback,
                      std::function<QByteArray (QByteArray)> ackCallback,
                      std::function<void
                      (QAbstractSocket::SocketError, QString)> errorCallback );

    void initData(int port, std::function<void (QByteArray)> parseCallback);
    void initData(int port,
                      std::function<void (QByteArray)> parseCallback,
                      std::function<QByteArray (QByteArray)> ackCallback );
    void initData(int port,
                      std::function<void (QByteArray)> parseCallback,
                      std::function<QByteArray (QByteArray)> ackCallback,
                      std::function<void
                      (QAbstractSocket::SocketError, QString)> errorCallback );
    void setPort(int port);
    void setParseCallback(std::function<void (QByteArray)> parseCallback);
    void setAckCallback(std::function<QByteArray (QByteArray)> ackCallback);
    void setErrorCallback(std::function<void
                      (QAbstractSocket::SocketError, QString)> errorCallback);

    ~UniversalListener();

    void initConnections();
    void startListener();

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
