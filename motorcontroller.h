/*
* =================BEGIN GPL LICENSE BLOCK=========================================
*  Author: Thomas George
*  PiRinter3D Copyright (C) 2016
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
#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H
#include <QtCore>
#include <QObject>
#include "steppermotor.h"

class MotorController : public QObject
{
	Q_OBJECT
public:
    StepperMotor::MotorDirection GetDirection(const int &Step);
    StepperMotor::MotorDirection GetReverseDirection(const int &Step);

    void StepMotor(StepperMotor &Motor, long Steps, const int &MSDelay);
    void StepMotors(StepperMotor &Motor1, long Steps1, StepperMotor &Motor2, long Steps2, const float &MSDelay);
    void StepMotors(StepperMotor &Motor1, long Steps1, StepperMotor &Motor2, long Steps2,
                    StepperMotor &Motor3, long Steps3, const float &MSDelay);
    void StepMotors(StepperMotor &Motor1, long Steps1, StepperMotor &Motor2, long Steps2,
                    StepperMotor &Motor3, long Steps3,  StepperMotor &Motor4, long Steps4, const float &MSDelay);
signals:
    void ReportMotorPosition(QString Name, const long Pos);
};

#endif // MOTORCONTROLLER_H
