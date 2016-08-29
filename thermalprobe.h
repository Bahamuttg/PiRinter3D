#ifndef THERMALPROBE_H
#define THERMALPROBE_H
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <mcp3004.h>
#include <math.h>
#include <stdio.h>
class ThermalProbe
{
private:
    unsigned int  _Channel, _PinBase, _TriggerPin;
    float _R1, _DefaultThermistorOHM, _DefaultThermistorTempK, _RefVoltage;
    int _TargetTemp, _ThermistorBeta, _CurrentTemp;
    //Debugging Code
    int FakerCtr;
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

    void SetTargetTemp(const int &CelsiusValue);

    int GetTargetTemp()
    {
        return _TargetTemp;
    }

    int GetCurrentTemp()
    {
        return _CurrentTemp;
    }

    void SetRefVoltage(const float &Value)
    {
        _RefVoltage = Value;
    }

    float GetRefVoltage()
    {
        return _RefVoltage;
    }

    void SetR1Resistance(const float &Ohms)
    {
        _R1 = Ohms;
    }

    float GetR1Resistance()
    {
        return _R1;
    }

    void SetDefaultThermistorResistance(const float &Ohms)
    {
        _DefaultThermistorOHM = Ohms;
    }

    float GetDefaultThermistorResistance()
    {
        return _DefaultThermistorOHM;
    }

    void SetDefaultThermistorTemp(const float &KelvinValue)
    {
        _DefaultThermistorTempK = KelvinValue;
    }

    float GetDefaultThermistorTempK()
    {
        return _DefaultThermistorTempK;
    }

    void SetThermistorBeta(const int &BetaValue)
    {
        _ThermistorBeta = BetaValue;
    }

    int GetThermistorBeta()
    {
        return _ThermistorBeta;
    }
};

#endif // THERMALPROBE_H
