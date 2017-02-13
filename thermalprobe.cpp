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
#include "thermalprobe.h"
#include "adccontroller.h"
#include <pigpio.h>
#include <math.h>
#include <stdio.h>

ThermalProbe::ThermalProbe(const double &RefV, const unsigned int &R1Ohm, const int &Beta, const unsigned int &ProbeOhm,
        const unsigned int &DefaultTemp, const unsigned int &TriggerPin, const int &ADCCh, ADCController *ADC)
{
	_ADCReader = ADC;
    _Channel = ADCCh;
    _TriggerPin = TriggerPin;
    _TargetTemp = 0;
    _CurrentTemp = 0;
    _FaultCount = 0;
    _FaultTempBuffer = 5;
    _FaultTolerance = 20;

    //Values that can be overridden by get/set.
    _RefVoltage = RefV;
    _R1 = R1Ohm;
    _DefaultThermistorOHM = ProbeOhm;
    _DefaultThermistorTempK = DefaultTemp;
    _ThermistorBeta = Beta;
    //==================================

    gpioSetMode(_TriggerPin, PI_OUTPUT);
	//Write pins low to start
    gpioWrite(_TriggerPin, PI_LOW);
}

ThermalProbe::~ThermalProbe()
{
    gpioWrite(_TriggerPin, PI_LOW);
}

int ThermalProbe::MeasureTemp()
{
    int ADCValue = 0, PreviousTemp;
    float VOut = 0, ThermOhms = 0;

    ADCValue = _ADCReader->GetChannelValue(_Channel); //this gives us the ADC value between 1024 (10bit)
    if(ADCValue <= 0 || _RefVoltage <= 0) //Don't want to divide by  0...
        return _CurrentTemp; //couldn't read this time... TODO set a boolean field so we can figure out if we need to try again right away.
    VOut = _RefVoltage * ((float)ADCValue / 1024); //this calculates the voltage differential over the thermistor (with respect to ground)
    if(VOut == _RefVoltage) //need to make sure we don't divide by zero again.
        return _CurrentTemp;
    PreviousTemp = _CurrentTemp;
    ThermOhms = (VOut * _R1) / (_RefVoltage - VOut); //this finds the current resistance of the thermistor
    //now that we have the resistance we can figure out how hot the thing is... by using the smart guys formula
    _CurrentTemp =
            ((_DefaultThermistorTempK * _ThermistorBeta) / log(_DefaultThermistorOHM / ThermOhms)
             / (_ThermistorBeta / log(_DefaultThermistorOHM / ThermOhms) - _DefaultThermistorTempK) -273.15);

    //Faulting algorithm...
    if(_CurrentTemp > 0)//Should never really read below zero.
    {
        if(_CurrentTemp > 315)
                _FaultCount ++;
        else if((_CurrentTemp > (PreviousTemp + _FaultTempBuffer) && ElementCurrentState == ThermalProbe::OFF)//We are climbing and we shouldn't be.
                ||  (_CurrentTemp < (PreviousTemp - _FaultTempBuffer) && ElementCurrentState == ThermalProbe::ON))//We are falling and we shouldn't be.
            _FaultCount ++;
        else
            _FaultCount = 0;
    }
    //=================

    if(_FaultCount < _FaultTolerance)
    {
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
    }
    else
        TriggerElement(ThermalProbe::OFF);
    return _CurrentTemp;
}

void ThermalProbe::TriggerElement(ElementState Value)
{
    if(Value == ThermalProbe::OFF)
        gpioWrite(_TriggerPin, PI_LOW);
    else if(!IsFaulted())
        gpioWrite(_TriggerPin, PI_HIGH);
    ElementCurrentState = Value;
}

void ThermalProbe::SetTargetTemp(const int &CelsiusValue )
{
    _TargetTemp = CelsiusValue;
    MeasureTemp();
}
