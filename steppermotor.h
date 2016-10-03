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
#ifndef STEPPERMOTOR_H
#define STEPPERMOTOR_H

#include <pigpio.h>
#include <string>

using namespace std;
namespace Stepper
{
    class StepperMotor;
}
class StepperMotor
{
private:
    unsigned int _Coil_1;
    unsigned int _Coil_2;
    unsigned int _Coil_3;
    unsigned int _Coil_4;
    unsigned int _Phase;
	unsigned int _MinPhaseDelay;
    unsigned int _StopPin;

    bool _IsNOTGated;
    bool _IsHalfStep;
    bool _Enabled;
    bool _IsInverted;
    bool _IsRotating;
    bool _IsAgainstEndstop;
	bool _IsExtCtrl;

    void CoilsOff();
    void InvertDirection();

public:
    enum MotorDirection
    {
        CLOCKWISE = 1,
        CTRCLOCKWISE = -1
    };

    string MotorName;
    MotorDirection Direction;
    long Position;
    volatile bool HoldPosition;
    explicit StepperMotor(int StepPin, int DirectionPin, int EnablePin, int MinPhaseDelay, string Name = "Default", int StopPin = 0);
    explicit StepperMotor(int Coil1, int Coil3, int MinPhaseDelay, string Name = "Default", int StopPin = 0);
    explicit StepperMotor(int Coil1, int Coil2, int Coil3, int Coil4, int MinPhaseDelay, bool IsHalfStep = false, string Name = "Default", int StopPin = 0);
    ~StepperMotor();

    void Rotate(MotorDirection Direction, long Steps, int MS_Delay);
    void Rotate(MotorDirection Direction, int MS_Delay);
    void StopRotation(const bool &Hold = true);
    bool MoveFromEndstop(const unsigned int &NumSteps = 1);
    void Enable();
    void Disable();
    void SetInverted(const bool &Arg = true);
    void SetNotGated(const bool &Arg = false);
	void SetEndstop(const unsigned int &EndStopPin);

    bool  HasEndstop()
    {
        if(_StopPin > 0)
            return true;
        else return false;
    }

    bool IsRotating()
    {
        return _IsRotating;
    }

    bool IsNotGated()
    {
        return _IsNOTGated;
    }

    bool IsInverted()
    {
        return _IsInverted;
    }

    bool IsAgainstStop()
    {
        return _IsAgainstEndstop;
    }

    int MaxSpeed()
    {
        return _MinPhaseDelay;
    }

protected:
    void PerformStep(MotorDirection);
};

#endif // STEPPERMOTOR_H
