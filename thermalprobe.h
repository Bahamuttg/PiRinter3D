#ifndef THERMALPROBE_H
#define THERMALPROBE_H
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <mcp3004.h>
class ThermalProbe
{
private:
    unsigned int  _Channel, _PinBase, _TriggerPin;

    int _TargetTemp;

public:
    ThermalProbe(const unsigned int &Channel, const unsigned int &SPIPinBase, const unsigned int &TriggerPin, const int &TargetTemp);
    ~ThermalProbe();

    enum ElementState
    {
        OFF = 0,
        ON = 1
    };

    ElementState ElementCurrentState;

    int MeasureTemp();

    void TriggerElement(ElementState);

    void SetTargetTemp(const int&Value);
};

#endif // THERMALPROBE_H
