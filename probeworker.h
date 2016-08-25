#ifndef PROBEWORKER_H
#define PROBEWORKER_H
#include <QtCore>
#include "thermalprobe.h"
class ProbeWorker : public QObject
{
    Q_OBJECT

private:
    bool _Suspend, _Terminate;
    ThermalProbe *_Probe;
    unsigned int _MS_ReadDelay;
public:
    ProbeWorker(ThermalProbe *Probe, const unsigned int &MS_ReadDelay);

public slots:
    void DoWork();

    void Suspend();

    void Resume();

    void Terminate();

signals:
    void ReportTemp(int);
    void WorkComplete();
    void Error(QString err);
};

#endif // PROBEWORKER_H
