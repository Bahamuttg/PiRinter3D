#include "probeworker.h"

ProbeWorker::ProbeWorker(ThermalProbe *Probe, const unsigned int &MS_ReadDelay)
{
    this->_Probe = Probe;
    this->_MS_ReadDelay = MS_ReadDelay;
}
void ProbeWorker::DoWork()
{
    //READ more about QThreading to handle a mutex or semaphore
    while(!_Terminate)
    {
        if(!_Suspend)
        {
            emit ReportTemp(_Probe->MeasureTemp());
            this->thread()->sleep(_MS_ReadDelay);
            if(_Terminate)
            {
                _Terminate = false;
                emit WorkComplete();
                return;
            }
        }
    }
    emit WorkComplete();
    _Terminate = false;
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
}
