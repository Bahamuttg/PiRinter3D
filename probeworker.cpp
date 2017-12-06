#include "probeworker.h"

ProbeWorker::ProbeWorker(ThermalProbe *Probe, const unsigned int &MS_ReadDelay, QObject *parent)
    :QThread(parent)
{
    this->_Terminate = false;
    this->_Suspend = false;
    this->_Probe = Probe;
    this->_MS_ReadDelay = MS_ReadDelay;
}
ProbeWorker::~ProbeWorker()
{
    _Mutex.lock();
    _Terminate = true;
    _Condition.wakeOne();
     this->_Probe->TriggerElement(ThermalProbe::OFF);
    _Mutex.unlock();
    wait();
}

void ProbeWorker::run()
{
    while(!_Terminate)
    {
        if(!_Suspend)
        {
            _Mutex.lock();
            TriggerProbeRead();
            emit ReportElementState(this->_Probe->ElementCurrentState);
            _Mutex.unlock();
        }
        msleep(_MS_ReadDelay);
    }
    this->_Probe->TriggerElement(ThermalProbe::OFF);
}

int ProbeWorker::TriggerProbeRead()
{
    int  AvgHldr = 0;
    int TempRead = 0;// _Probe->MeasureTemp();
    QList<int> Readings;
    for (int i = 0; i < 7; i++)
        Readings << _Probe->MeasureTemp();
    qSort(Readings.begin(), Readings.end());

    for (int i = 0; i < 2; i++)//remove four elements
    {
        Readings.removeFirst();
        Readings.removeLast();
    }

    for (int i = 0; i < Readings.count(); i++)
        AvgHldr += Readings[i];
    if(AvgHldr > 0 && Readings.count() > 0)
        TempRead = AvgHldr / Readings.count();
    else
        TempRead = 0;
    emit ReportTemp(TempRead);
    if(TempRead < this->_Probe->GetTargetTemp())
        this->_Probe->TriggerElement(ThermalProbe::ON);
    else
        this->_Probe->TriggerElement(ThermalProbe::OFF);

    return TempRead;
}

void ProbeWorker::Suspend()
{
    this->_Suspend = true;
}

void ProbeWorker::Resume()
{
    this->_Suspend = false;
}

void ProbeWorker::Stop()
{
    this->_Terminate = true;
    this->_Probe->TriggerElement(ThermalProbe::OFF);
    emit ReportElementState(this->_Probe->ElementCurrentState);
}
