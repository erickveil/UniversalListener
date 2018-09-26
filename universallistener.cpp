#include "universallistener.h"

UniversalListener::UniversalListener(QObject *parent) : QObject(parent)
{

}

UniversalListener::~UniversalListener()
{
    if (_connection != nullptr) {
        delete _connection;
        _connection = nullptr;
    }
}

void UniversalListener::initConnections()
{
    connect(&_server, SIGNAL(acceptError(QAbstractSocket::SocketError)),
            this, SLOT(_eventListenerAcceptError(QAbstractSocket::SocketError)));
    connect(&_server, SIGNAL(newConnection()),
            this, SLOT(_eventListenerNewConnection()));
    _isConnected = true;
}

void UniversalListener::startListener(int port)
{
    if (!_isConnected) {
        // TODO: handle problem
        qDebug() << "Listener not initialized.";
        return;
    }

    bool isListening = _server.listen(QHostAddress::Any, port);

    if (!isListening) {
        // TODO: Handle error
        qDebug() << "Listening error: " << _server.errorString();
        _server.close();
        return;
    }
    else {
        qDebug() << "Listening on " << _server.serverAddress().toString()
                 << ":" << QString::number(_server.serverPort());
    }
}

void UniversalListener::_eventListenerAcceptError(QAbstractSocket::SocketError err)
{
    qDebug() << "Connection accept error: " + _server.errorString();
    // TODO: Run error callback.
}

void UniversalListener::_eventListenerNewConnection()
{
    qDebug() << "Event new connection.";

    if (_connection != nullptr) {
        if (_connection->isOpen()) { _connection->close(); }
        delete _connection;
        _connection = nullptr;
    }

    _connection = _server.nextPendingConnection();

    connect(_connection, SIGNAL(connected()),
            this, SLOT(_eventSocketConnected()) );
    connect(_connection, SIGNAL(disconnected()),
            this, SLOT(_eventSocketDisconnected()));
    connect(_connection, QOverload<QAbstractSocket::SocketError>::
            of(&QAbstractSocket::error),
            [=](QAbstractSocket::SocketError socketError) {
        _eventSocketError(socketError);
    });
    connect(_connection, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(_eventSocketStateChanged()));
    connect(_connection, SIGNAL(aboutToClose()),
            this, SLOT(_eventIODeviceAboutToClose()));
    connect(_connection, SIGNAL(bytesWritten(qint64)),
            this, SLOT(_eventIODeviceBytesWritten(qint64)));
    connect(_connection, SIGNAL(readyRead()),
            this, SLOT(_eventIODeviceReadyRead()));


}

void UniversalListener::_eventSocketConnected()
{
    qDebug() << "Event connected.";

}

void UniversalListener::_eventSocketDisconnected()
{
    qDebug() << "Event disconnected.";

}

void UniversalListener::_eventSocketError(QAbstractSocket::SocketError err)
{
    qDebug() << "Event error: " << _connection->errorString();

}

void UniversalListener::_eventSocketStateChanged()
{
    qDebug() << "Event State Changed.";

}

void UniversalListener::_eventIODeviceAboutToClose()
{
    qDebug() << "Event About to Close.";

}

void UniversalListener::_eventIODeviceBytesWritten(qint64 bytes)
{
    qDebug() << "Event Bytes Written: " << QString::number(bytes);

}

void UniversalListener::_eventIODeviceReadyRead()
{
    // Read data
    qDebug() << "Event Ready Read.";
    int bytesAvailable = _connection->bytesAvailable();
    qDebug() << "Starting bytes available: " << QString::number(bytesAvailable);

    QByteArray receivedData;
    receivedData.clear();

    while (_connection->bytesAvailable() > 0) {
        receivedData.append(_connection->readAll());
        qDebug() << "IO Device Read: " + QString::number(receivedData.length());
    }

    qDebug() << "Data received: " << receivedData;

    // Run Ack callback if exists
    // Write (ack or echo if no ack callback) response to socket.
    _connection->write(receivedData);

    // Tell the client we're done.
    _connection->flush();
    _connection->close();

    // Run Parse callback if exists

}
