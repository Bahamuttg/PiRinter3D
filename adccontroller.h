#ifndef ADCCONTROLLER_H
#define ADCCONTROLLER_H

#include <QObject>
#include <QMutex>

#include "pigpio.h"

class ADCController : public QObject
{
private:
    Q_OBJECT

    unsigned int _Clock, _CS, _MISO, _MOSI;

    QMutex _Mutex;

    rawSPI_t rawSPI();

public:
    explicit ADCController(const unsigned int &Clock, const unsigned int &MOSI, const unsigned int &MISO, const unsigned int &CS, QObject *parent = 0);
    ~ADCController();

    int GetChannelValue(const unsigned int &Channel);

signals:

public slots:

};

#endif // ADCCONTROLLER_H
