#ifndef PROBEWORKER_H
#define PROBEWORKER_H
#include <QtCore>
#include <QThread>
#include "thermalprobe.h"
class ProbeWorker : public QThread
{
    Q_OBJECT

private:
    bool _Suspend, _Terminate;
    ThermalProbe *_Probe;
    unsigned int _MS_ReadDelay;
    QMutex _Mutex;
    QWaitCondition _Condition;

protected:
    void run();

public:
    ProbeWorker(ThermalProbe *Probe, const unsigned int &MS_ReadDelay, QObject *parent =0);
    ~ProbeWorker();

    int TriggerProbeRead();

    void SetTargetTemp(const int &CelsiusValue)
    {
        this->_Probe->SetTargetTemp(CelsiusValue);
    }

    int GetTargetTemp()
    {
        return this->_Probe->GetTargetTemp();
    }

    int GetCurrentTemp()
    {
        return this->_Probe->GetCurrentTemp();
    }

public slots:
    void Suspend();

    void Resume();

    void Stop();

signals:
    void ReportElementState(bool);
    void ReportTemp(int);
    void Error(QString err);
};

#endif // PROBEWORKER_H
