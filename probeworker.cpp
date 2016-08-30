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
    int TempRead = _Probe->MeasureTemp();
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

void ProbeWorker::Terminate()
{
    this->_Terminate = true;
    this->_Probe->TriggerElement(ThermalProbe::OFF);
    emit ReportElementState(this->_Probe->ElementCurrentState);
    emit ReportTemp(0);
}
