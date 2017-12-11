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
#include "steppermotor.h"
#include <pigpio.h>
#include <string>
StepperMotor::StepperMotor(unsigned int StepPin, unsigned int DirectionPin, unsigned int EnablePin, unsigned int MinPhaseDelay, string Name, unsigned int StopPin)
{
    this->MotorName = Name;
    this->_Coil_1 = StepPin;
    this->_Coil_2 = DirectionPin;
    this->_Coil_3 = EnablePin;
    this->_Coil_4 = 0;
    this->_IsNOTGated = false;
    this->_IsExtCtrl = true;
    this->_IsHalfStep = false;
    this->_Enabled = true;
    this->_IsInverted = false;
    this->_IsRotating = false;
    this->_MinPhaseDelay = MinPhaseDelay;
    this->_StopPin = StopPin;

    //Setup Pin->Coil Mappings
    gpioSetMode(_Coil_1, PI_OUTPUT);
    gpioSetMode(_Coil_2, PI_OUTPUT);
    gpioSetMode(_Coil_3, PI_OUTPUT);
    if(_StopPin > 0)
    {
        gpioSetMode(_StopPin, PI_INPUT);
        this->_IsAgainstEndstop = !gpioRead(_StopPin);
    }
    else
        this->_IsAgainstEndstop = false;
    //write pins to the Off state
    gpioWrite(_Coil_1, PI_LOW);
    gpioWrite(_Coil_3, PI_HIGH);
    this->_Phase = 0;
    this->Direction = CLOCKWISE;
    this->Position = 0;
    this->HoldPosition = true;
}
StepperMotor::StepperMotor(unsigned int Coil1, unsigned int Coil3, unsigned int MinPhaseDelay, string Name, unsigned int StopPin)
{
    this->MotorName = Name;
    this->_Coil_1 = Coil1;
    this->_Coil_2 = 0;
    this->_Coil_3 = Coil3;
    this->_Coil_4 = 0;
    this->_IsNOTGated = true;
    this->_IsExtCtrl = false;
    this->_IsHalfStep = false;
    this->_Enabled = true;
    this->_IsInverted = false;
    this->_IsRotating = false;
    this->_MinPhaseDelay = MinPhaseDelay;
    this->_StopPin = StopPin;

    //Setup Pin->Coil Mappings
    gpioSetMode(_Coil_1, PI_OUTPUT);
    gpioSetMode(_Coil_3, PI_OUTPUT);
    if(_StopPin > 0)
    {
        gpioSetMode(_StopPin, PI_INPUT);
        this->_IsAgainstEndstop = !gpioRead(_StopPin);
    }
    else
        this->_IsAgainstEndstop = false;
    //Write pins low to start
    CoilsOff();
    this->_Phase = 0;
    this->Direction = CLOCKWISE;
    this->Position = 0;
    this->HoldPosition = true;
}

StepperMotor::StepperMotor(unsigned int Coil1, unsigned int Coil2, unsigned int Coil3, unsigned int Coil4, unsigned int MinPhaseDelay, bool IsHalfStep, string Name, unsigned int StopPin)
{
    this->MotorName = Name;
    this->_Coil_1 = Coil1;
    this->_Coil_2 = Coil2;
    this->_Coil_3 = Coil3;
    this->_Coil_4 = Coil4;
    this->_IsNOTGated = false;
    this->_IsExtCtrl = false;
    this->_IsHalfStep = IsHalfStep;
    this->_Enabled = true;
    this->_IsInverted = false;
    this->_IsRotating = false;
    this->_MinPhaseDelay = MinPhaseDelay;
    this->_StopPin = StopPin;

    //Setup Pin->Coil Mappings
    gpioSetMode(_Coil_1, PI_OUTPUT);
    gpioSetMode(_Coil_2, PI_OUTPUT);
    gpioSetMode(_Coil_3, PI_OUTPUT);
    gpioSetMode(_Coil_4, PI_OUTPUT);
    if(_StopPin > 0)
    {
        gpioSetMode(_StopPin, PI_INPUT);
        this->_IsAgainstEndstop = !gpioRead(_StopPin);
    }
    else
        this->_IsAgainstEndstop = false;
    //Write pins low to start
    CoilsOff();
    this->_Phase = 0;
    this->Direction = CLOCKWISE;
    this->Position = 0;
    this->HoldPosition = true;
}

StepperMotor::~StepperMotor()
{
    CoilsOff();
}

void StepperMotor::Rotate(MotorDirection Direction, long Steps)
{
    if (_Enabled)
    {
        if(Steps < 0)
            Steps *= -1;
        this->_IsRotating = true;
        for (int i = 0; i < Steps; i++)
        {
            if(_StopPin > 0 && !gpioRead(_StopPin))
            {
                CoilsOff();
                this->_IsRotating = false;
                this->_IsAgainstEndstop = true;
                return;
            }
            PerformStep(Direction);
        }
        //If we're not holding the motor position let's release it.
        if (!HoldPosition)
            CoilsOff();
    }
    this->_IsRotating = false;
}

void StepperMotor::Rotate(MotorDirection Direction, long Steps, unsigned int MS_Delay)
{
    if (_Enabled)
    {
        if(Steps < 0)
            Steps *= -1;
        //this is the minimum speed at which we can switch the coils.
        if(MS_Delay < this->_MinPhaseDelay)
            MS_Delay = this->_MinPhaseDelay;
        this->_IsRotating = true;
        for (int i = 0; i < Steps; i++)
        {
            if(_StopPin > 0 && !gpioRead(_StopPin))
            {
                CoilsOff();
                this->_IsRotating = false;
                this->_IsAgainstEndstop = true;
                return;
            }
            PerformStep(Direction);
           if(MS_Delay >  999999)
            {
                int Sec = (int)(MS_Delay / 1000000);
                gpioSleep(PI_TIME_RELATIVE, Sec, (MS_Delay) - (MS_Delay * 1000000));
            }
           else
               gpioSleep(PI_TIME_RELATIVE, 0, MS_Delay);
        }
        //If we're not holding the motor position let's release it.
        if (!HoldPosition)
            CoilsOff();
    }
    this->_IsRotating = false;
}

void StepperMotor::Rotate(MotorDirection Direction)
{
    //This method will block indefinitely.
    //It's up to the programmer to thread it properly.
    this->_IsRotating = true;
    while (_Enabled)
    {
        if(_StopPin > 0 && !gpioRead(_StopPin))
        {
            CoilsOff();
            this->_IsRotating = false;
            this->_IsAgainstEndstop = true;
            return;
        }
        PerformStep(Direction);
        gpioSleep(PI_TIME_RELATIVE, 0, _MinPhaseDelay);// * 1000);
    }
    //If we're not holding the motor position let's release it.
    if (!HoldPosition)
        CoilsOff();
    this->_IsRotating = false;
}

void StepperMotor::StopRotation(const bool &Hold)
{
    this->_Enabled = false;
    if (!Hold)
        CoilsOff();
}

bool StepperMotor::MoveFromEndstop(const unsigned int &NumSteps)
{
    //If it's against the endstop take action, otherwise just return true.
    if(_StopPin > 0 && !gpioRead(_StopPin))
    {
        InvertDirection();
        for (unsigned int i = 0; i < NumSteps; i++)
        {
            PerformStep(this->Direction);
            gpioSleep(PI_TIME_RELATIVE, 0, this->_MinPhaseDelay);// * 10000);
        }
        InvertDirection();
        //Check to see that we moved off the stop.
        if(!gpioRead(_StopPin))
        {
            this->_IsAgainstEndstop = true;
            return false;
        }
        else
        {
            this->_IsAgainstEndstop = false;
            return true;
        }
    }
    else
        return true;
}

void StepperMotor::PerformStep(MotorDirection Direction)
{
    if (_Enabled)
    {
        if (_IsInverted)
        {
            if (Direction == CLOCKWISE)
                this->Direction = CTRCLOCKWISE;
            else
                this->Direction = CLOCKWISE;
        }
        else
            this->Direction = Direction;

        if(_IsExtCtrl)
        {
            if(_Coil_3 > 0)
                gpioWrite(_Coil_3, PI_LOW); //Enable the motor if it was previously disabled.
            if(this->Direction == CTRCLOCKWISE)
                gpioWrite(_Coil_2, PI_LOW);//Coil 2 is the direction pin when using external controller.
            else
                gpioWrite(_Coil_2, PI_HIGH);

            gpioWrite(_Coil_1, PI_LOW);
            gpioSleep(PI_TIME_RELATIVE, 0, 10);//Maximum delay never less than 1.9 microseconds.
            gpioWrite(_Coil_1, PI_HIGH);
            this->Position += this->Direction;
        }
        else
        {
            //full step sequence. maximum torque
            const unsigned int FullStep[4][4] = { {1,0,1,0},{0,1,1,0},{0,1,0,1},{1,0,0,1} };
            //half-step sequence. double resolution. But the torque of the stepper motor is not constant
            const unsigned int HalfStep[8][4] = { {1,0,0,0},{1,0,1,0},{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,1,0,1},{0,0,0,1},{1,0,0,1} };
            //Record our target phase
            int TargetPhase = (this->_Phase + this->Direction);

            //Check if we need to reset the phase.
            if (!_IsHalfStep)
            {
                if (this->Direction == CLOCKWISE && TargetPhase > 3)
                    TargetPhase = 0;
                else if (this->Direction == CTRCLOCKWISE && TargetPhase < 0)
                    TargetPhase = 3;
            }
            else
            {
                if (this->Direction == CLOCKWISE && TargetPhase > 7)
                    TargetPhase = 0;
                else if (this->Direction == CTRCLOCKWISE && TargetPhase < 0)
                    TargetPhase = 7;
            }

            //Set coils based upon phase shift, direction,and stepping style.
            if (!_IsHalfStep)
            {
                gpioWrite(_Coil_1, FullStep[TargetPhase][0]);
                gpioWrite(_Coil_3, FullStep[TargetPhase][2]);
                if (!_IsNOTGated)
                {
                    gpioWrite(_Coil_2, FullStep[TargetPhase][1]);
                    gpioWrite(_Coil_4, FullStep[TargetPhase][3]);
                }
            }
            else
            {
                gpioWrite(_Coil_1, HalfStep[TargetPhase][0]);
                gpioWrite(_Coil_3, HalfStep[TargetPhase][2]);
                if (!_IsNOTGated)
                {
                    gpioWrite(_Coil_2, HalfStep[TargetPhase][1]);
                    gpioWrite(_Coil_4, HalfStep[TargetPhase][3]);
                }
            }
            //Update motor status.
            this->_Phase = TargetPhase;
            this->Position += this->Direction;
            //Account for inversion
            this->Direction = Direction;
        }
    }
}

void StepperMotor::Enable()
{
    this->_Enabled = true;
}

void StepperMotor::Disable()
{
    this->_Enabled = false;
    CoilsOff();
}

void StepperMotor::SetEndstop(const unsigned int &EndStopPin)
{
    if(_StopPin > 0)
    {
        gpioSetMode(_StopPin, PI_OUTPUT);
        gpioWrite(_StopPin, PI_LOW);
    }
    this->_StopPin =  EndStopPin;
    if(EndStopPin > 0)
        gpioSetMode(_StopPin, PI_INPUT);
}

void StepperMotor::CoilsOff()
{
    if(_IsExtCtrl)
        gpioWrite(_Coil_3, PI_HIGH);
    else
    {
        gpioWrite(_Coil_1, PI_LOW);
        gpioWrite(_Coil_3, PI_LOW);
        if(!this->_IsNOTGated)
        {
            gpioWrite(_Coil_2, PI_LOW);
            gpioWrite(_Coil_4, PI_LOW);
        }
    }
}

void StepperMotor::InvertDirection()
{
    if(this->_IsInverted)
        this->_IsInverted = false;
    else
        this->_IsInverted = true;
}

void StepperMotor::SetNotGated(const bool &Arg)
{
    if(_IsExtCtrl)
        return;
    this->_IsNOTGated = Arg;
    this->_IsHalfStep = false;
}

void StepperMotor::SetHoldOnIdle(const bool &Arg)
{
    this->HoldPosition = Arg;
}
