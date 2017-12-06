#include "fanworker.h"

FanWorker::FanWorker(Fan  *FanObj, const unsigned int &MS_ReadDelay, QObject *parent)
    :QThread(parent)
{
    this->_Terminate = false;
    this->_Suspend = false;
    this->_Fan = FanObj;
    this->_MS_ReadDelay = MS_ReadDelay;
}

FanWorker::~FanWorker()
{
    _Mutex.lock();
    _Terminate = true;
    _Condition.wakeOne();
     this->_Fan->PowerOff();
    _Mutex.unlock();
    this->wait();
}

void FanWorker::run()
{
    while(!_Terminate)
    {
        if(!_Suspend)
        {
            _Mutex.lock();
            _Fan->PowerOn();
            emit ReportDutyCycle(this->_Fan->GetDutyCycle());
            emit ReportRPM(this->_Fan->GetFanRPM());
            _Mutex.unlock();
        }
        msleep(_MS_ReadDelay);
    }
    this->_Fan->PowerOff();
}

int FanWorker::ReadRPM()
{

}

void FanWorker::Suspend()
{
    this->_Suspend = true;
}

void FanWorker::Resume()
{
    this->_Suspend = false;
}

void FanWorker::Stop()
{
    this->_Terminate = true;
    this->_Fan->PowerOff();
}
