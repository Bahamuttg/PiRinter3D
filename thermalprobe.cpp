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
#include "thermalprobe.h"
#include "adccontroller.h"
#include <pigpio.h>
#include <math.h>
#include <stdio.h>

ThermalProbe::ThermalProbe(ADCController *ADC, const unsigned int &Channel, const unsigned int &TriggerPin, const int &TargetTemp)
{
	_ADCReader = ADC;
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

    gpioSetMode(_TriggerPin, PI_OUTPUT);

    FakerCtr =0;
}

ThermalProbe::~ThermalProbe()
{
    gpioWrite(_TriggerPin, PI_LOW);
}

int ThermalProbe::MeasureTemp()
{
//									NEED A MUTEX FOR THIS METHOD WHEN USED IN WORKER...
//    int ADCValue = 0;
//    float VOut = 0, ThermOhms = 0;

        int ADCValue = _ADCReader->GetChannelValue(_Channel); //this gives us the ADC value between 1024 (10bit)
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
        gpioWrite(_TriggerPin, PI_LOW);
    else
        gpioWrite(_TriggerPin, PI_HIGH);
    ElementCurrentState = Value;
}

void ThermalProbe::SetTargetTemp(const int &CelsiusValue )
{
    _TargetTemp = CelsiusValue;
    MeasureTemp();
}
