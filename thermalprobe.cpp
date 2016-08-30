#include "thermalprobe.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <math.h>
#include <stdio.h>

ThermalProbe::ThermalProbe(const unsigned int &Channel, const unsigned int &SPIPinBase, const unsigned int &TriggerPin, const int &TargetTemp)
{
    _PinBase = SPIPinBase;
    _Channel = Channel;
    _TriggerPin = TriggerPin;
    _TargetTemp = TargetTemp;
    _CurrentTemp = 0;

    //Values that can be overridden by get/set.
    _RefVoltage = 3.3;
    _R1 = 1000;
    _DefaultThermistorOHM = 10000;
    _DefaultThermistorTempK = 298.15;
    _ThermistorBeta = 3974;
    //==================================

    pinMode(_TriggerPin, OUTPUT);
    //mcp3004Setup(_PinBase, _Channel);
    FakerCtr =0;
}

ThermalProbe::~ThermalProbe()
{
    digitalWrite(_TriggerPin, LOW);
}

int ThermalProbe::MeasureTemp()
{
//									NEED A MUTEX FOR THIS METHOD WHEN USED IN WORKER...
//    int ADCValue = 0;
//    float VOut = 0, ThermOhms = 0;

//     ADCValue = analogRead(_PinBase); //this gives us the ADC value between 1024 (10bit)
//     VOut = _RefVoltage * ((float)ADCValue / 1024); //this calculates the voltage differential over the thermistor (with respect to ground)
//     ThermOhms = (VOut * _R1) / (_RefVoltage - VOut); //this finds the current resistance of the thermistor
//     //now that we have the resistance we can figure out how hot the thing is... by using the smart guys formula
//     _CurrentTemp =
//             ((_DefaultThermistorTempK * _ThermistorBeta) / log(_DefaultThermistorOHM / ThermOhms) / (_ThermistorBeta / log(_DefaultThermistorOHM / ThermOhms) - _DefaultThermistorTempK) -273.15);


    if(_CurrentTemp > _TargetTemp)
    {
        if(ElementCurrentState == ThermalProbe::ON)
            TriggerElement(ThermalProbe::OFF);
    }
    else if(_CurrentTemp < _TargetTemp)
    {
            if(ElementCurrentState == ThermalProbe::OFF)
                TriggerElement(ThermalProbe::ON);
    }
    else
        TriggerElement(ThermalProbe::OFF);

    if(FakerCtr < _TargetTemp + 1)
        FakerCtr ++;
    else if(FakerCtr >= _TargetTemp + 1)
        FakerCtr -= 3;
    _CurrentTemp = FakerCtr;

	return _CurrentTemp;
}

void ThermalProbe::TriggerElement(ElementState Value)
{
    if(Value == ThermalProbe::OFF)
        digitalWrite(_TriggerPin, LOW);
    else
        digitalWrite(_TriggerPin, HIGH);
    ElementCurrentState = Value;
}

void ThermalProbe::SetTargetTemp(const int &CelsiusValue )
{
    _TargetTemp = CelsiusValue;
    MeasureTemp();
}
