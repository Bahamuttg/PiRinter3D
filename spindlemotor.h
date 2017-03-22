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
#ifndef SPINDLEMOTOR_H
#define SPINDLEMOTOR_H

#include <pigpio.h>
#include <string>


using namespace std;
namespace Spindle
{
    class SpindleMotor;
}

class SpindleMotor
{
private:
    unsigned int _H_BR1;
    unsigned int _H_BR2;
    unsigned int _EnablePin;
    unsigned int _SensorPin;
    unsigned int _RPM;
    unsigned int _TopSpeed;
    unsigned int _DutyCycle;
    float _PWMOffset; //This is the usable part of our PWM spectrum from the top side of the duty cycle.
    float _RPMBeta;//This is the beta value for the log curve of PWM to RPM

    pthread_t *RPMThread;

    bool _Enabled;
    bool _IsInverted;
    bool _IsRotating;
    bool _IsHBridge;

    void InvertDirection();

public:
    enum MotorDirection
    {
        CLOCKWISE = 1,
        CTRCLOCKWISE = -1
    };
    string MotorName;
    MotorDirection Direction;

    explicit SpindleMotor(unsigned int EnergizeTrigger, unsigned int EnablePin = 0, const unsigned int &SensorPin = 0,string Name = "Default");
    explicit SpindleMotor(unsigned int H_BR1, unsigned int H_BR2, unsigned int EnablePin = 0, const unsigned int &SensorPin = 0, string Name = "Default");

    ~SpindleMotor();

    void Rotate(MotorDirection Direction, const unsigned int &RPM = 0);
    void StopRotation(const bool &Hold = true);
    void SetRPM(int RPM);

    void MotorOff();
    void Brake();
    void Coast();
    void Enable();
    void Disable();
    void SetInverted(const bool &Arg = true);

    bool IsRotating()
    {
        return _IsRotating;
    }

    bool IsInverted()
    {
        return _IsInverted;
    }

    int MaxSpeed()
    {
        return 0;
    }

protected:
    void PowerOn();//Needs to stay protected so it can only be called in this class or derivitives.
};

#endif // SPINDLEMOTOR_H
