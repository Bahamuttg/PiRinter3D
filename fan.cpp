#include "fan.h"

Fan::Fan(const unsigned int &TriggerPin, const unsigned int &SensorPin)
{
    this->_TriggerPin = TriggerPin;
    this->_SensorPin = SensorPin;
    this->_DutyCycle = 1;

    if(_SensorPin > 0)
        gpioSetMode(_SensorPin, PI_INPUT);
    gopioSetMode(_TriggerPin, PI_OUTPUT);
    gpioWrite(_TriggerPin, PI_LOW);
}

Fan::~Fan()
{
    if(_SensorPin > 0)
    {
        gpioSetMode(_SensorPin, PI_OUTPUT);
        gpioWrite(_SensorPin, PI_LOW);
    }
    gpioWrite(_TriggerPin, PI_LOW);
}

void Fan::PowerOn()
{
    _PowerState = Fan::ON;
    if(_DutyCycle < 1)
        gpioPWM(_TriggerPin, _DutyCycle);
    else
        gpioWrite(_TriggerPin, PI_HIGH);
}

void Fan::PowerOff()
{
    _PowerState = Fan::OFF;
    gpioWrite(_TriggerPin, PI_LOW);
}

int Fan::GetFanRPM()
{
    //do some raw wave shit here.
}

void Fan::SetRPM(const unsigned int &RPM)
{
    this->_TargetRPM = RPM;
    if(this->_PowerState == Fan::ON)
        PowerOn();
//    if(GetFanRPM() < _TargetRPM - _RPMBuffer)
//    {
//         _DutyCycle -= .1;
//    }
//    else if(GetFanRPM() > _TargetRPM + _RPMBuffer)
//    {
//        _DutyCycle -= .1;
//    }
}
