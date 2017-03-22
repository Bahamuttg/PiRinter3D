#ifndef FAN_H
#define FAN_H

#include <pigpio.h>

class Fan
{
private:
    unsigned int _TriggerPin, _SensorPin, _TargetRPM, _PowerState, _RPMBuffer;
    float _DutyCycle;
    pthread_t *RPMThread;

public:

    enum FanState
    {
        OFF = 0,
        ON = 1
    };

    Fan(const unsigned int &TriggerPin, const unsigned int &SensorPin = 0);
    ~Fan();

    void PowerOn();
    void PowerOff();

    int GetFanRPM();
    void SetRPM(const unsigned int &RPM);

    int GetDutyCycle()
    {
        return _DutyCycle;
    }

    void SetDutyCycle(const float &Value)
    {
        _DutyCycle = Value;
        if(_PowerState == Fan::ON)
            PowerOn();
    }

    void SetRPMBuffer(const unsigned int &Buffer)
    {
        _RPMBuffer = Buffer;
    }

    int GetRPMBuffer()
    {
        return _RPMBuffer;
    }


};

#endif // FAN_H
