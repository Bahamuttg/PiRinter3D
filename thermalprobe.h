/*
* =================BEGIN GPL LICENSE BLOCK=========================================
* 
*  This program is free software; you can redistribute it and/or 
*  modify it under the terms of the GNU General Public License 
*  as published by the Free Software Foundation; either version 2 
*  of the License, or (at your option) any later version. 
* 
*  This program is distributed in the hope that it will be useful, 
*  but WITHOUT ANY WARRANTY; without even the implied warranty of 
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
*  GNU General Public License for more details. 
* 
*  You should have received a copy of the GNU General Public License 
*  along with this program; if not, write to the Free Software Foundation, 
*  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. 
* 
* =================END GPL LICENSE BLOCK=========================================
*/
#ifndef THERMALPROBE_H
#define THERMALPROBE_H
#include <pigpio.h>
#include <math.h>
#include <stdio.h>
#include "adccontroller.h"
class ThermalProbe
{
private:
	ADCController *_ADCReader; 
	//ADC needs the channel to read on. 
	//It needs to be part of the interpreter so we can control the mutex needed because we have more than one probe.
	//It will control the waveform and return an int value between 0 - 1024;
	
    unsigned int  _Channel,  _TriggerPin;
    float _R1, _DefaultThermistorOHM, _DefaultThermistorTempK, _RefVoltage;
    int _TargetTemp, _ThermistorBeta, _CurrentTemp;
    //Debugging Code
    int FakerCtr;
public:
    ThermalProbe(ADCController *ADC, const unsigned int &Channel, const unsigned int &TriggerPin, const int &TargetTemp);
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
