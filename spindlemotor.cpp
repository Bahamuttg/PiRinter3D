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
#include "spindlemotor.h"
# include <string>
#include <pigpio.h>

SpindleMotor::SpindleMotor(unsigned int EnergizeTrigger, unsigned int EnablePin, const unsigned int &SensorPin, string Name)
{
    this->MotorName =Name;
    this->_H_BR1 = EnergizeTrigger;
    this->_H_BR2 = 0;
    this->_EnablePin = EnablePin;
    this->_SensorPin = SensorPin;
    this->_DutyCycle = 100;
    this->_Enabled = true;
    this->_IsHBridge = false;
    this->_IsInverted = false;
    this->_IsRotating = false;
    this->Direction = CLOCKWISE;
    gpioSetMode(_H_BR1, PI_OUTPUT);
    if(_EnablePin > 0)
    {
        gpioSetMode(_EnablePin, PI_OUTPUT);
        gpioSetPWMrange(_EnablePin, 100);
    }
    else
         gpioSetPWMrange(_H_BR1, 100);
    MotorOff();
}

SpindleMotor::SpindleMotor(unsigned int H_BR1, unsigned int H_BR2, unsigned int EnablePin, const unsigned int &SensorPin, string Name)
{
    this->MotorName =Name;
    this->_H_BR1 = H_BR1;
    this->_H_BR2 = H_BR2;
    this->_EnablePin = EnablePin;
    this->_SensorPin = SensorPin;
    this->_DutyCycle = 100;
    this->_Enabled = true;
    this->_IsHBridge = true;
    this->_IsInverted = false;
    this->_IsRotating = false;
    this->Direction = CLOCKWISE;
    gpioSetMode(_H_BR1, PI_OUTPUT);
    gpioSetMode(_H_BR2, PI_OUTPUT);
    MotorOff();
    if(_EnablePin > 0)
    {
        gpioSetMode(_EnablePin, PI_OUTPUT);
        gpioSetPWMrange(_EnablePin, 100);
    }
    else
    {
         gpioSetPWMrange(_H_BR1, 100);
         gpioSetPWMrange(_H_BR2, 100);
    }
}

void SpindleMotor::Rotate(MotorDirection Direction, const unsigned int &RPM)
{
    if (_Enabled)
    {
        this->_IsRotating = true;
        if (_IsInverted)
        {
            if (Direction == CLOCKWISE)
                this->Direction = CTRCLOCKWISE;
            else
                this->Direction = CLOCKWISE;
        }
        else
            this->Direction = Direction;
        if(RPM > 0)
        {
            SetRPM(RPM);
            PowerOn();
        }
        //Account for inversion
        this->Direction = Direction;
    }
}

void SpindleMotor::PowerOn()
{
    if(_IsHBridge)
    {
        if(_EnablePin > 0)
        {
            if(_DutyCycle < 100)
                gpioPWM(_EnablePin, _DutyCycle);
            else
                gpioWrite(_EnablePin, PI_HIGH);

            if(this->Direction == CLOCKWISE)
            {
                if(_DutyCycle < 100)
                    gpioPWM(_H_BR1, _DutyCycle);
                else
                    gpioWrite(_H_BR1, PI_HIGH);
                gpioWrite(_H_BR2, PI_LOW);
            }
            else
            {
                if(_DutyCycle < 100)
                    gpioPWM(_H_BR2, _DutyCycle);
                else
                    gpioWrite(_H_BR2, PI_HIGH);
                gpioWrite(_H_BR2, PI_LOW);
            }
        }
        else
        {
            if(this->Direction == CLOCKWISE)
            {
                gpioWrite(_H_BR1, PI_HIGH);
                gpioWrite(_H_BR2, PI_LOW);
            }
            else
            {
                gpioWrite(_H_BR1, PI_LOW);
                gpioWrite(_H_BR2, PI_HIGH);
            }
        }
    }
    else
    {
        if(_EnablePin > 0)
        {
            if(_DutyCycle < 100)
                gpioPWM(_EnablePin, _DutyCycle);
            else
                gpioWrite(_EnablePin, PI_HIGH);
            gpioWrite(_H_BR1, PI_HIGH);
        }
        else
        {
            if(_DutyCycle < 100)
                gpioPWM(_H_BR1, _DutyCycle);
            else
                gpioWrite(_H_BR1, PI_HIGH);
        }
    }

    //    if(_SensorPin > 0)
    //        RPMThread =  gpioStartThread(SampleSpeed, this);

}

void SpindleMotor::SetRPM(int RPM)
{
    if(RPM > _TopSpeed)
        RPM = _TopSpeed;
    float percent = float(RPM / _TopSpeed);
    _DutyCycle = 100;
}

void SpindleMotor::Brake()
{
    if(_IsHBridge)
    {
        gpioWrite(_H_BR1, PI_HIGH);
        gpioWrite(_H_BR2, PI_HIGH);
    }
    else
        Coast();
    this->_IsRotating = false;
}
void SpindleMotor::Coast()
{
    gpioWrite(_H_BR1, PI_LOW);
    gpioWrite(_H_BR2, PI_LOW);
}
void SpindleMotor::Enable()
{
    this->_Enabled = true;
    if(_EnablePin > 0)
    {
        if(_DutyCycle < 100)
            gpioPWM(_EnablePin, _DutyCycle);
        else
            gpioWrite(_EnablePin, PI_HIGH);
    }
}

void SpindleMotor::Disable()
{
    this->_Enabled = false;
    MotorOff();
    this->_IsRotating = false;
}

void SpindleMotor::MotorOff()
{
        gpioWrite(_H_BR1, PI_LOW);
        gpioWrite(_H_BR2, PI_LOW);
        if(!_Enabled && _EnablePin > 0)
            gpioWrite(_EnablePin, PI_LOW);
        this->_IsRotating = false;
}

void SpindleMotor::InvertDirection()
{
    if(this->_IsInverted)
        this->_IsInverted = false;
    else
        this->_IsInverted = true;
}
