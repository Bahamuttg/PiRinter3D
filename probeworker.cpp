#include "probeworker.h"

ProbeWorker::ProbeWorker(ThermalProbe *Probe, const unsigned int &MS_ReadDelay)
{
    this->_Probe = Probe;
    this->_MS_ReadDelay = MS_ReadDelay;
    this->_DelayTimer = new QTimer(this);
    this->_DelayTimer->setInterval(_MS_ReadDelay);
    connect(this->_DelayTimer, SIGNAL(timeout()), this, SLOT(DoWork()));
}
ProbeWorker::~ProbeWorker()
{
    delete _DelayTimer;
}

void ProbeWorker::DoWork()
{
    if(!_Suspend)
        _DelayTimer->start();
    else
        _DelayTimer->stop();
}

int ProbeWorker::TriggerProbeRead()
{
    int temp = _Probe->MeasureTemp();
    emit ReportTemp(temp);
    return temp;
}

void ProbeWorker::Suspend()
{
    this->_Suspend = true;
    DoWork();
}

void ProbeWorker::Resume()
{
    this->_Suspend = false;
    DoWork();
}
