#ifndef ADCCONTROLLER_H
#define ADCCONTROLLER_H

#include <QObject>
#include <QMutex>

#include "pigpio.h"

class ADCController : public QObject
{
private:
    Q_OBJECT

    int _Clock, _CS, _MISO, _MOSI;

    QMutex _Mutex;

public:
    explicit ADCController(const int &Clock, const int &MISO, const int &MOSI, const int &CS, QObject *parent = 0);
    ~ADCController();

    int GetChannelValue(const unsigned int &Channel);

    int GetChannelAverage(const unsigned int &Channel, const int &Reads);

signals:

public slots:

};

#endif // ADCCONTROLLER_H
