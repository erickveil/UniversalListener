#ifndef UNIVERSALLISTENER_H
#define UNIVERSALLISTENER_H

#include <QObject>

class UniversalListener : public QObject
{
    Q_OBJECT
public:
    explicit UniversalListener(QObject *parent = nullptr);

signals:

public slots:
};

#endif // UNIVERSALLISTENER_H