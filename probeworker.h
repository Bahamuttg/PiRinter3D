#ifndef PROBEWORKER_H
#define PROBEWORKER_H
#include <QtCore>
#include <QTimer>
#include "thermalprobe.h"
class ProbeWorker : public QObject
{
    Q_OBJECT

private:
    bool _Suspend, _Terminate;
    ThermalProbe *_Probe;
    unsigned int _MS_ReadDelay;
    QTimer *_DelayTimer;

    int TriggerProbeRead();

public:
    ProbeWorker(ThermalProbe *Probe, const unsigned int &MS_ReadDelay);
    ~ProbeWorker();

public slots:
    void DoWork();

    void Suspend();

    void Resume();

signals:
    void ReportTemp(int);
    void Error(QString err);
};

#endif // PROBEWORKER_H
