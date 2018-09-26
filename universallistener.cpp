#include "universallistener.h"

UniversalListener::UniversalListener(QObject *parent) : QObject(parent)
{

}

UniversalListener::UniversalListener(int port)
{
    setPort(port);
}

UniversalListener::UniversalListener(
        int port, std::function<void (QByteArray)> parseCallback)
{
    initData(port,parseCallback);
}

UniversalListener::UniversalListener(
        int port, std::function<void (QByteArray)> parseCallback,
        std::function<QByteArray (QByteArray)> ackCallback)
{
    initData(port,parseCallback, ackCallback);
}

UniversalListener::UniversalListener(int port,
        std::function<void (QByteArray)> parseCallback,
        std::function<QByteArray (QByteArray)> ackCallback,
        std::function<void (QAbstractSocket::SocketError, QString)>
                                     errorCallback)
{
    initData(port,parseCallback, ackCallback, errorCallback);
}

void UniversalListener::initData(
        int port, std::function<void (QByteArray)> parseCallback)
{
    setPort(port);
    setParseCallback(parseCallback);
    initConnections();
}

void UniversalListener::initData(
        int port,
        std::function<void (QByteArray)> parseCallback,
        std::function<QByteArray (QByteArray)> ackCallback)
{
    setPort(port);
    setParseCallback(parseCallback);
    setAckCallback(ackCallback);
    initConnections();
}

void UniversalListener::initData(int port,
        std::function<void (QByteArray)> parseCallback,
        std::function<QByteArray (QByteArray)> ackCallback,
        std::function<void (QAbstractSocket::SocketError, QString)> errorCallback)
{
    setPort(port);
    setParseCallback(parseCallback);
    setAckCallback(ackCallback);
    setErrorCallback(errorCallback);
    initConnections();
}

void UniversalListener::setPort(int port)
{
    _port = port;
}

void UniversalListener::setParseCallback(
        std::function<void (QByteArray)> parseCallback)
{
    _parseCallback = parseCallback;
}

void UniversalListener::setAckCallback(
        std::function<QByteArray (QByteArray)> ackCallback)
{
    _ackCallback = ackCallback;
}

void UniversalListener::setErrorCallback(
        std::function<void (QAbstractSocket::SocketError, QString)> errorCallback)
{
    _errorCallback = errorCallback;
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
    if (_isSignalsConnected) { return; }
    connect(&_server, SIGNAL(acceptError(QAbstractSocket::SocketError)),
            this, SLOT(_eventListenerAcceptError(QAbstractSocket::SocketError)));
    connect(&_server, SIGNAL(newConnection()),
            this, SLOT(_eventListenerNewConnection()));
    _isSignalsConnected = true;
}

void UniversalListener::startListener()
{
    if (!_isSignalsConnected || _port == -1) {
        // TODO: handle problem
        qDebug() << "Listener not initialized.";
        return;
    }

    if (_server.isListening()) { _server.close(); }

    bool isListening = _server.listen(QHostAddress::Any, _port);

    if (!isListening) {
        // TODO: Handle error
        qDebug() << "Listening error: " << _server.errorString();
        if (_errorCallback) {
            _errorCallback(_server.serverError(), _server.errorString());
        }
        _server.close();
        return;
    }
    else {
        qDebug() << "Listening on " << _server.serverAddress().toString()
                 << ":" << QString::number(_server.serverPort());
    }
}

void UniversalListener::stopListener()
{
    if (_server.isListening()) { _server.close(); }
}

void UniversalListener::_eventListenerAcceptError(QAbstractSocket::SocketError err)
{
    qDebug() << "Connection accept error: " + _server.errorString();
    // TODO: Run error callback.
    if (_errorCallback) { _errorCallback(err, _server.errorString()); }
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
    Q_UNUSED(err);
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

    QByteArray ack;
    ack.clear();

    if (_ackCallback) { ack = _ackCallback(receivedData); }
    else { ack = receivedData; }

    _connection->write(ack);

    _connection->flush();
    _connection->close();

    if (_parseCallback) {
        _parseCallback(receivedData);
    }
}
