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
	
	volatile bool _IsNOTGated;
    volatile bool _IsHalfStep;
    volatile bool _Enabled;
    volatile bool _IsInverted;
    volatile bool _IsRotating;

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
    explicit StepperMotor(int Coil1, int Coil3, int MinPhaseDelay, string Name = "Default");
    explicit StepperMotor(int Coil1, int Coil2, int Coil3, int Coil4, int MinPhaseDelay, bool IsHalfStep = false, string Name = "Default");
    ~StepperMotor();

    void Rotate(MotorDirection Direction, long Steps, int MS_Delay);
    void Rotate(MotorDirection Direction, int MS_Delay);
    void StopRotation(const bool &Hold = true);
    void Enable();
    void Disable();
    void SetInverted(const bool &Arg = true);
    void SetNotGated(const bool &Arg = false);


protected:
    void PerformStep(MotorDirection);
};

#endif // STEPPERMOTOR_H
