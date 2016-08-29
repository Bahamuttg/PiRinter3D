#include "motorcontroller.h"
#include "steppermotor.h"

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
void MotorController::StepMotor(StepperMotor &Motor, long Steps, const int &Speed)
{
    if(Speed > 0)
    {
        Motor.Rotate(GetDirection(Steps), qAbs(Steps), (float)(1/(float)Speed));
        qDebug()<< QString::fromStdString(Motor.MotorName) << " Steps -- " << QString::number(Steps) << " Speed --  " << QString::number((float)(1/(float)Speed));
    }
}
//Control two stepper motors simultainously with a specified speed and direction.
void MotorController::StepMotors(StepperMotor &Motor1, long Steps1, StepperMotor &Motor2, long Steps2, const int &Speed)
{
	StepperMotor::MotorDirection Dir1 = GetDirection(Steps1);
	StepperMotor::MotorDirection Dir2 = GetDirection(Steps2);

	Steps1 = qAbs(Steps1);
	Steps2 = qAbs(Steps2);
    //Iterator is always the largest stepping value.
    int Iterator = qMax(Steps1, Steps2);

	//init holders for ratio calc
	float M1Holder = 0, M2Holder = 0;

	//init ctr's for error correction
	int M1ctr = 0, M2ctr = 0;

    //Calculate ratios.
	float M1Ratio = ((float)Steps1 / (float)Iterator);
	float M2Ratio = ((float)Steps2 / (float)Iterator);


    //Old code, Converted to calc over the rate of travel NOT time it takes for the move.
    //float T = (Steps1 + Steps2) / Speed
    //Time delay per step
    //float deltaTime = Time / ((float)Steps1 + (float)Steps2);

    //Total Rate of Travel
    float Hyp = qSqrt(qPow(Steps1, 2) + qPow(Steps2, 2));
    float BaseDelay = (float)(1/(float)Speed);
    float SpeedDelta = Hyp / (((float)Steps1 + (float)Steps2));
    float deltaTime = BaseDelay * SpeedDelta;

    qDebug()<< QString::fromStdString(Motor1.MotorName) << " Steps -- " << QString::number(Steps1) << " Speed --  " << QString::number(deltaTime);
    qDebug()<< QString::fromStdString(Motor2.MotorName) << " Steps -- " << QString::number(Steps2) << " Speed --  " << QString::number(deltaTime);

	//Rotate Motors...
	for (int i = 0; i < Iterator; i++)
	{
		M1Holder += M1Ratio;
		M2Holder += M2Ratio;

		if (M1Holder >= 1)
		{
			M1Holder -= 1;
			M1ctr++;
			Motor1.Rotate(Dir1, 1, deltaTime);
		}
		if (M2Holder >= 1)
		{
			M2Holder -= 1;
			M2ctr++;
			Motor2.Rotate(Dir2, 1, deltaTime);
		}
    }
    //Clean-up any missed steps due to float rounding errors.
    if (M1ctr < Steps1)
        Motor1.Rotate(Dir1, 1, deltaTime);
    if (M2ctr < Steps2)
        Motor2.Rotate(Dir2, 1, deltaTime);
    //Clean-up any oversteps we may have done due to rounding errors.
    if (M1ctr > Steps1)
        Motor1.Rotate(GetReverseDirection(Dir1), 1, deltaTime);
    if (M2ctr > Steps2)
        Motor2.Rotate(GetReverseDirection(Dir2), 1, deltaTime);
}
//Control three stepper motors simultainously with a specified speed and direction.
//Controlling three or more motors requires some complex microstepping alogrithms.
void MotorController::StepMotors(StepperMotor &Motor1, long Steps1, StepperMotor &Motor2, long Steps2, StepperMotor &Motor3, long Steps3, const int &Speed)
{
	StepperMotor::MotorDirection Dir1 = GetDirection(Steps1);
	StepperMotor::MotorDirection Dir2 = GetDirection(Steps2);
	StepperMotor::MotorDirection Dir3 = GetDirection(Steps3);

	Steps1 = qAbs(Steps1);
	Steps2 = qAbs(Steps2);
	Steps3 = qAbs(Steps3);
    //Iterator is always the largest stepping value.
    int Iterator = qMax(Steps1, qMax(Steps2, Steps3));

	//init holders for ratio calc
	float M1Holder = 0, M2Holder = 0, M3Holder = 0;

    //init ctr's for error correction
	int M1ctr = 0, M2ctr = 0, M3ctr = 0;

    //Calculate ratios.
	float M1Ratio = ((float)Steps1 / (float)Iterator);
	float M2Ratio = ((float)Steps2 / (float)Iterator);
	float M3Ratio = ((float)Steps3 / (float)Iterator);


    float Hyp = qSqrt(qPow(Steps1, 2) + qPow(Steps2, 2));
    float BaseDelay = (float)(1/(float)Speed);
    float SpeedDelta = Hyp / (((float)Steps1 + (float)Steps2));
    float deltaTime = BaseDelay * SpeedDelta;

    qDebug()<< QString::fromStdString(Motor1.MotorName) << " Steps -- " << QString::number(Steps1) << " Speed --  " << QString::number(deltaTime);
    qDebug()<< QString::fromStdString(Motor2.MotorName) << " Steps -- " << QString::number(Steps2) << " Speed --  " << QString::number(deltaTime);
    qDebug()<< QString::fromStdString(Motor3.MotorName) << " Steps -- " << QString::number(Steps3) << " Speed --  " << QString::number(deltaTime);

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
			Motor1.Rotate(Dir1, 1, deltaTime);
		}
		if (M2Holder >= 1)
		{
			M2Holder -= 1;
			M2ctr++;
			Motor2.Rotate(Dir2, 1, deltaTime);
		}
		if (M3Holder >= 1)
		{
			M3Holder -= 1;
			M3ctr++;
			Motor3.Rotate(Dir3, 1, deltaTime);
		}
    }
    //Clean-up any missed steps due to float rounding errors.
    if (M1ctr < Steps1)
        Motor1.Rotate(Dir1, 1, deltaTime);
    if (M2ctr < Steps2)
        Motor2.Rotate(Dir2, 1, deltaTime);
    if (M3ctr < Steps3)
        Motor3.Rotate(Dir3, 1, deltaTime);
    //Clean-up any oversteps we may have done due to rounding errors.
    if (M1ctr > Steps1)
        Motor1.Rotate(GetReverseDirection(Dir1), 1, deltaTime);
    if (M2ctr > Steps2)
        Motor2.Rotate(GetReverseDirection(Dir2), 1, deltaTime);
    if (M3ctr > Steps3)
        Motor3.Rotate(GetReverseDirection(Dir3), 1, deltaTime);
}
void MotorController::StepMotors(StepperMotor &Motor1, long Steps1, StepperMotor &Motor2, long Steps2,
                StepperMotor &Motor3, long Steps3, StepperMotor &Motor4, long Steps4, const int &Speed)
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

    //init holders for ratio calc
    float M1Holder = 0, M2Holder = 0, M3Holder = 0, M4Holder = 0;

    //init ctr's for error correction
    int M1ctr = 0, M2ctr = 0, M3ctr = 0, M4ctr = 0;

    //Calculate ratios.
    float M1Ratio = ((float)Steps1 / (float)Iterator);
    float M2Ratio = ((float)Steps2 / (float)Iterator);
    float M3Ratio = ((float)Steps3 / (float)Iterator);
    float M4Ratio = ((float)Steps4 / (float)Iterator);



    float Hyp = qSqrt(qPow(Steps1, 2) + qPow(Steps2, 2));
    float BaseDelay = (float)(1/(float)Speed);
    float SpeedDelta = Hyp / (((float)Steps1 + (float)Steps2));
    float deltaTime = BaseDelay * SpeedDelta;

    qDebug()<< QString::fromStdString(Motor1.MotorName) << " Steps -- " << QString::number(Steps1) << " Speed --  " << QString::number(deltaTime);
    qDebug()<< QString::fromStdString(Motor2.MotorName) << " Steps -- " << QString::number(Steps2) << " Speed --  " << QString::number(deltaTime);
    qDebug()<< QString::fromStdString(Motor3.MotorName) << " Steps -- " << QString::number(Steps3) << " Speed --  " << QString::number(deltaTime);
    qDebug()<< QString::fromStdString(Motor4.MotorName) << " Steps -- " << QString::number(Steps4) << " Speed --  " << QString::number(deltaTime);

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
            Motor1.Rotate(Dir1, 1, deltaTime);
        }
        if (M2Holder >= 1)
        {
            M2Holder -= 1;
            M2ctr++;
            Motor2.Rotate(Dir2, 1, deltaTime);
        }
        if (M3Holder >= 1)
        {
            M3Holder -= 1;
            M3ctr++;
            Motor3.Rotate(Dir3, 1, deltaTime);
        }
        if (M4Holder >= 1)
        {
            M4Holder -= 1;
            M4ctr++;
            Motor4.Rotate(Dir4, 1, deltaTime);
        }
    }
    //Clean-up any missed steps due to float rounding errors.
    if (M1ctr < Steps1)
        Motor1.Rotate(Dir1, 1, deltaTime);
    if (M2ctr < Steps2)
        Motor2.Rotate(Dir2, 1, deltaTime);
    if (M3ctr < Steps3)
        Motor3.Rotate(Dir3, 1, deltaTime);
    if (M4ctr < Steps4)
        Motor4.Rotate(Dir4, 1, deltaTime);
    //Clean-up any oversteps we may have done due to rounding errors.
    if (M1ctr > Steps1)
        Motor1.Rotate(GetReverseDirection(Dir1), 1, deltaTime);
    if (M2ctr > Steps2)
        Motor2.Rotate(GetReverseDirection(Dir2), 1, deltaTime);
    if (M3ctr > Steps3)
        Motor3.Rotate(GetReverseDirection(Dir3), 1, deltaTime);
    if (M4ctr > Steps4)
        Motor4.Rotate(GetReverseDirection(Dir4), 1, deltaTime);
}
