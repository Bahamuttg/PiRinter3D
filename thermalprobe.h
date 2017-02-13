/*
* =================BEGIN GPL LICENSE BLOCK=========================================
*  Author: Thomas George
*  PiRinter3D Copyright (C) 2016
*
*  This program is free software; you can redistribute it and/or 
*  modify it under the terms of the GNU General Public License 
*  as published by the Free Software Foundation; either version 3 
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
	
    unsigned int  _Channel,  _TriggerPin;
    float _R1, _DefaultThermistorOHM, _DefaultThermistorTempK, _RefVoltage;
    int _TargetTemp, _ThermistorBeta, _CurrentTemp, _FaultTempBuffer, _FaultTolerance, _FaultCount;

public:
    ThermalProbe(const double &RefV, const unsigned int &R1Ohm, const int &Beta, const unsigned int &ProbeOhm,
                 const unsigned int &DefaultTemp, const unsigned int &TriggerPin, const int &ADCCh, ADCController *ADC);
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

    void SetFaultTempBuffer(const int &Value)
    {
        _FaultTempBuffer = Value;
    }

    int GetFaultTempBuffer()
    {
        return _FaultTempBuffer;
    }

    void SetFaultTolerance(const int &Value)
    {
        _FaultTolerance = Value;
    }

    int GetFaultTolerance()
    {
        return _FaultTolerance;
    }

    void Reset()
    {
        this->_FaultCount = 0;
        TriggerElement(ThermalProbe::OFF);
    }

    bool IsFaulted()
    {
        return _FaultCount >= _FaultTolerance;
    }
};

#endif // THERMALPROBE_H
