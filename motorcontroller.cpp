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
#include "motorcontroller.h"
#include "steppermotor.h"
#include <QtCore>
#include <QObject>

//Returns the intended direction of the motor based off of the int signing.
StepperMotor::MotorDirection MotorController::GetDirection(const int &Step)
{
	if (Step > 0)
		return StepperMotor::CLOCKWISE;
	else
		return StepperMotor::CTRCLOCKWISE;
}
//Returns the inverse direction of the motor based off of the int signing.
StepperMotor::MotorDirection MotorController::GetReverseDirection(const int &Step)
{
    if (Step > 0)
        return StepperMotor::CTRCLOCKWISE;
    else
        return StepperMotor::CLOCKWISE;
}
//Control a single stepper motor with a specified speed and direction.
void MotorController::StepMotor(StepperMotor &Motor, long Steps, const int &MSDelay)
{
    Motor.Rotate(GetDirection(Steps), qAbs(Steps), MSDelay);
    emit ReportMotorPosition(QString::fromStdString(Motor.MotorName), Motor.Position );
    //qDebug()<< QString::fromStdString(Motor.MotorName) << " Steps -- " << QString::number(Steps) << " Speed --  " << QString::number(MSDelay);
}
//Control two stepper motors simultaneously with a specified speed and direction.
void MotorController::StepMotors(StepperMotor &Motor1, long Steps1, StepperMotor &Motor2, long Steps2, const float &MSDelay)
{
	StepperMotor::MotorDirection Dir1 = GetDirection(Steps1);
	StepperMotor::MotorDirection Dir2 = GetDirection(Steps2);

	Steps1 = qAbs(Steps1);
	Steps2 = qAbs(Steps2);
    //Iterator is always the largest stepping value.
    int Iterator = qMax(Steps1, Steps2);

    //Slowest motor phase delay
    int LongestDelay = qMax(Motor1.MaxSpeed(), Motor2.MaxSpeed());

	//init holders for ratio calc
	float M1Holder = 0, M2Holder = 0;

	//init ctr's for error correction
	int M1ctr = 0, M2ctr = 0;

    //Calculate ratios.
	float M1Ratio = ((float)Steps1 / (float)Iterator);
	float M2Ratio = ((float)Steps2 / (float)Iterator);

    //Total Rate of Travel speed is in Milliseconds
    float Hyp = qSqrt(qPow(Steps1, 2) + qPow(Steps2, 2));
    float DelayDelta = Hyp / (((float)Steps1 + (float)Steps2));
    float Delay = MSDelay / DelayDelta;

    //qDebug()<< "X Steps -- " << QString::number(Steps1) <<  " Y Steps -- " << QString::number(Steps2) << " Speed --  " << QString::number(Delay);

	//Rotate Motors...
	for (int i = 0; i < Iterator; i++)
	{
		M1Holder += M1Ratio;
		M2Holder += M2Ratio;

		if (M1Holder >= 1)
		{
			M1Holder -= 1;
			M1ctr++;
            Motor1.Rotate(Dir1, 1);
		}
		if (M2Holder >= 1)
		{
			M2Holder -= 1;
			M2ctr++;
            Motor2.Rotate(Dir2, 1);
		}
        if(Delay < LongestDelay)
            gpioSleep(PI_TIME_RELATIVE, 0, LongestDelay);// * 1000);
        else
            gpioSleep(PI_TIME_RELATIVE, 0, Delay * 1000);
        emit ReportMotorPosition(QString::fromStdString(Motor1.MotorName), Motor1.Position );
        emit ReportMotorPosition(QString::fromStdString(Motor2.MotorName), Motor2.Position );
    }
    //Clean-up any missed steps due to float rounding errors.
    if (M1ctr < Steps1)
        Motor1.Rotate(Dir1, 1, LongestDelay);
    if (M2ctr < Steps2)
        Motor2.Rotate(Dir2, 1, LongestDelay);
    //Clean-up any oversteps we may have done due to rounding errors.
    if (M1ctr > Steps1)
        Motor1.Rotate(GetReverseDirection(Dir1), 1, LongestDelay);
    if (M2ctr > Steps2)
        Motor2.Rotate(GetReverseDirection(Dir2), 1, LongestDelay);
}
//Control three stepper motors simultaneously with a specified speed and direction.
//Controlling three or more motors requires some complex micro-stepping algorithms.
void MotorController::StepMotors(StepperMotor &Motor1, long Steps1, StepperMotor &Motor2, long Steps2, StepperMotor &Motor3, long Steps3, const float &MSDelay)
{
	StepperMotor::MotorDirection Dir1 = GetDirection(Steps1);
	StepperMotor::MotorDirection Dir2 = GetDirection(Steps2);
	StepperMotor::MotorDirection Dir3 = GetDirection(Steps3);

	Steps1 = qAbs(Steps1);
	Steps2 = qAbs(Steps2);
	Steps3 = qAbs(Steps3);
    //Iterator is always the largest stepping value.
    int Iterator = qMax(Steps1, qMax(Steps2, Steps3));

    //Slowest motor phase delay
    int LongestDelay = qMax(Motor1.MaxSpeed(), qMax(Motor2.MaxSpeed(), Motor3.MaxSpeed()));

	//init holders for ratio calc
	float M1Holder = 0, M2Holder = 0, M3Holder = 0;

    //init ctr's for error correction
	int M1ctr = 0, M2ctr = 0, M3ctr = 0;

    //Calculate ratios.
	float M1Ratio = ((float)Steps1 / (float)Iterator);
	float M2Ratio = ((float)Steps2 / (float)Iterator);
	float M3Ratio = ((float)Steps3 / (float)Iterator);


    //Total Rate of Travel
    float Hyp = qSqrt(qPow(Steps1, 2) + qPow(Steps2, 2));
    float DelayDelta = Hyp / (((float)Steps1 + (float)Steps2));
    float Delay = MSDelay / DelayDelta;

    //qDebug()<< "X Steps -- " << QString::number(Steps1) <<  " Y Steps -- " << QString::number(Steps2) << " Ext Steps -- " << QString::number(Steps3) << " Speed --  " << QString::number(Delay);

	//Rotate Motors...
    for (int i = 0; i < Iterator; i++)
	{
		M1Holder += M1Ratio;
		M2Holder += M2Ratio;
		M3Holder += M3Ratio;

		if (M1Holder >= 1)
		{
			M1Holder -= 1;
			M1ctr++;
            Motor1.Rotate(Dir1, 1);
		}
		if (M2Holder >= 1)
		{
			M2Holder -= 1;
			M2ctr++;
            Motor2.Rotate(Dir2, 1);
		}
		if (M3Holder >= 1)
		{
			M3Holder -= 1;
			M3ctr++;
            Motor3.Rotate(Dir3, 1);
		}
        if(Delay < LongestDelay)
            gpioSleep(PI_TIME_RELATIVE, 0, LongestDelay);// * 1000);
        else
            gpioSleep(PI_TIME_RELATIVE, 0, Delay * 1000);
        emit ReportMotorPosition(QString::fromStdString(Motor1.MotorName), Motor1.Position );
        emit ReportMotorPosition(QString::fromStdString(Motor2.MotorName), Motor2.Position );
        emit ReportMotorPosition(QString::fromStdString(Motor3.MotorName), Motor3.Position );
    }
    //Clean-up any missed steps due to float rounding errors.
    if (M1ctr < Steps1)
        Motor1.Rotate(Dir1, 1, LongestDelay);
    if (M2ctr < Steps2)
        Motor2.Rotate(Dir2, 1, LongestDelay);
    if (M3ctr < Steps3)
        Motor3.Rotate(Dir3, 1, LongestDelay);
    //Clean-up any oversteps we may have done due to rounding errors.
    if (M1ctr > Steps1)
        Motor1.Rotate(GetReverseDirection(Dir1), 1, LongestDelay);
    if (M2ctr > Steps2)
        Motor2.Rotate(GetReverseDirection(Dir2), 1, LongestDelay);
    if (M3ctr > Steps3)
        Motor3.Rotate(GetReverseDirection(Dir3), 1, LongestDelay);
}
void MotorController::StepMotors(StepperMotor &Motor1, long Steps1, StepperMotor &Motor2, long Steps2,
                StepperMotor &Motor3, long Steps3, StepperMotor &Motor4, long Steps4, const float &MSDelay)
{
    StepperMotor::MotorDirection Dir1 = GetDirection(Steps1);
    StepperMotor::MotorDirection Dir2 = GetDirection(Steps2);
    StepperMotor::MotorDirection Dir3 = GetDirection(Steps3);
    StepperMotor::MotorDirection Dir4 = GetDirection(Steps4);

    Steps1 = qAbs(Steps1);
    Steps2 = qAbs(Steps2);
    Steps3 = qAbs(Steps3);
    Steps4 = qAbs(Steps4);

    //Iterator is always the largest stepping value.
    int Iterator = qMax(Steps1, qMax(Steps2, qMax(Steps3, Steps4)));

    //Slowest motor phase delay
    int LongestDelay = qMax(Motor1.MaxSpeed(), qMax(Motor2.MaxSpeed(), qMax(Motor3.MaxSpeed(), Motor4.MaxSpeed())));

    //init holders for ratio calc
    float M1Holder = 0, M2Holder = 0, M3Holder = 0, M4Holder = 0;

    //init ctr's for error correction
    int M1ctr = 0, M2ctr = 0, M3ctr = 0, M4ctr = 0;

    //Calculate ratios.
    float M1Ratio = ((float)Steps1 / (float)Iterator);
    float M2Ratio = ((float)Steps2 / (float)Iterator);
    float M3Ratio = ((float)Steps3 / (float)Iterator);
    float M4Ratio = ((float)Steps4 / (float)Iterator);



    //Total Rate of Travel
    float Hyp = qSqrt(qPow(Steps1, 2) + qPow(Steps2, 2));
    float DelayDelta = Hyp / (((float)Steps1 + (float)Steps2));
    float Delay = MSDelay / DelayDelta;

    //Rotate Motors...
    for (int i = 0; i < Iterator; i++)
    {
        M1Holder += M1Ratio;
        M2Holder += M2Ratio;
        M3Holder += M3Ratio;
        M4Holder += M4Ratio;

        if (M1Holder >= 1)
        {
            M1Holder -= 1;
            M1ctr++;
            Motor1.Rotate(Dir1, 1);
        }
        if (M2Holder >= 1)
        {
            M2Holder -= 1;
            M2ctr++;
            Motor2.Rotate(Dir2, 1);
        }
        if (M3Holder >= 1)
        {
            M3Holder -= 1;
            M3ctr++;
            Motor3.Rotate(Dir3, 1);
        }
        if (M4Holder >= 1)
        {
            M4Holder -= 1;
            M4ctr++;
            Motor4.Rotate(Dir4, 1);
        }
        if(Delay < LongestDelay)
            gpioSleep(PI_TIME_RELATIVE, 0, LongestDelay);// * 1000);
        else
            gpioSleep(PI_TIME_RELATIVE, 0, Delay * 1000);
        emit ReportMotorPosition(QString::fromStdString(Motor1.MotorName), Motor1.Position );
        emit ReportMotorPosition(QString::fromStdString(Motor2.MotorName), Motor2.Position );
        emit ReportMotorPosition(QString::fromStdString(Motor3.MotorName), Motor3.Position );
        emit ReportMotorPosition(QString::fromStdString(Motor4.MotorName), Motor4.Position );
    }
    //Clean-up any missed steps due to float rounding errors.
    if (M1ctr < Steps1)
        Motor1.Rotate(Dir1, 1, LongestDelay);
    if (M2ctr < Steps2)
        Motor2.Rotate(Dir2, 1, LongestDelay);
    if (M3ctr < Steps3)
        Motor3.Rotate(Dir3, 1, LongestDelay);
    if (M4ctr < Steps4)
        Motor4.Rotate(Dir4, 1, LongestDelay);
    //Clean-up any oversteps we may have done due to rounding errors.
    if (M1ctr > Steps1)
        Motor1.Rotate(GetReverseDirection(Dir1), 1, LongestDelay);
    if (M2ctr > Steps2)
        Motor2.Rotate(GetReverseDirection(Dir2), 1, LongestDelay);
    if (M3ctr > Steps3)
        Motor3.Rotate(GetReverseDirection(Dir3), 1, LongestDelay);
    if (M4ctr > Steps4)
        Motor4.Rotate(GetReverseDirection(Dir4), 1, LongestDelay);
}
