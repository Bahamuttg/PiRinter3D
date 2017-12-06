#ifndef FANWORKER_H
#define FANWORKER_H
#include <QtCore>
#include <QThread>
#include"fan.h"

class FanWorker : public QThread
{
private:
    Q_OBJECT

    bool _Suspend, _Terminate;
    Fan *_Fan;
    unsigned int _MS_ReadDelay;
    QMutex _Mutex;
    QWaitCondition _Condition;
protected:
    void run();

public:
    FanWorker(Fan *FanObj, const unsigned int &MS_ReadDelay, QObject *parent =0);
    ~FanWorker();

    int ReadRPM();

    void SetDutyCycle(const int &Percent)
    {
        this->_Fan->SetDutyCycle(Percent);
    }

    void SetRPM(const int &RPM)
    {
        this->_Fan->SetRPM(RPM);
    }

    int GetDutyCycle()
    {
        return this->_Fan->GetDutyCycle();
    }

    int GetRPM()
    {
        return this->_Fan->GetFanRPM();
    }

public slots:
    void Suspend();

    void Resume();

    void Stop();

signals:
    void ReportDutyCycle(bool);
    void ReportRPM(int);
    void Error(QString err);
};
#endif // FANWORKER_H
