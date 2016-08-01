#include "motorcontroller.h"
#include "steppermotor.h"
#include <math.h>

//Private::Disallow Creating instances of this class.
MotorController::MotorController()
{}

//Returns the greatest common divisor of two integers.
int GetGCD(int val1, int val2)
{
	return val2 == 0 ? val1 : GetGCD(val2, (val1 % val2));
}
//Returns the GCD of three integers.
int GetGCD(int val1, int val2, int val3)
{
	return GetGCD(val1, GetGCD(val2, val3));
}
//Returns the least common multiple of two integers.
int GetLCM(int val1, int val2)
{
	return val1*val2 / GetGCD(val1, val2);
}
//Returns the LCM of three integers.
int GetLCM(int val1, int val2, int val3)
{
    return GetLCM(GetLCM(val1, val2), val3);
}
//Returns the intended direction of the motor based off of the int signing.
StepperMotor::MotorDirection GetDirection(int Step)
{
	if (Step > 0)
		return StepperMotor::CLOCKWISE;
	else
		return StepperMotor::CTRCLOCKWISE;
}

//Control a single stepper motor with a specified speed and direction.
void StepMotor(StepperMotor *Motor, long Steps, int Speed)
{
	//Total Time
	float Time = (float)Steps / (float)Speed;
	//Time delay per step
    float deltaTime = Time / (float)Steps;
	//Rotate...
    Motor->Rotate(GetDirection(Steps), qAbs(Steps), deltaTime);
}
//Control two stepper motors simultainously with a specified speed and direction.
void StepMotors(StepperMotor *Motor1, long Steps1, StepperMotor *Motor2, long Steps2, int Speed)
{
	long TotalMicrosteps = 0; //Total microsteps in the series.
	long MicrostepMotor1 = 0;
	long MicrostepMotor2 = 0;

	StepperMotor::MotorDirection Dir1 = GetDirection(Steps1);
	StepperMotor::MotorDirection Dir2 = GetDirection(Steps2);
	
    Steps1 = qAbs(Steps1);
    Steps2 = qAbs(Steps2);

	//Calculate Microstepping
	if (Steps1 == 0)
	{
		TotalMicrosteps = Steps2;
		//We're going to be stepping every time since this is the only motor to turn.
		MicrostepMotor2 = 1;
		//We aren't moving the first motor so lets make sure 
		//its microstep value is greater than the TotalMicrosteps.
		MicrostepMotor1 = TotalMicrosteps * 10;
	}
	else if (Steps2 == 0)
	{
		//See above comments.
		TotalMicrosteps = Steps1;
		MicrostepMotor1 = 1;
		MicrostepMotor2 = TotalMicrosteps * 10;
	}
	else
	{
		//If we're stepping two of them we need to find the 
		//least common multiple of the steps. We'll then iterate through
		//that value looking for the modulus values that yield 0.
		TotalMicrosteps = GetLCM(Steps1, Steps2);
		MicrostepMotor1 = TotalMicrosteps / Steps1;
		MicrostepMotor2 = TotalMicrosteps / Steps2;
	}

	//Total Time
    float Time = qSqrt(qPow(Steps1, 2) + qPow(Steps2, 2)) / Speed;
	//Time delay per step
    float deltaTime = Time / ((float)Steps1 + (float)Steps2);

    int incrementer = GetGCD(Steps1, Steps2);
    int initalvalue = qMin(Steps1,Steps2);

    //Rotate Motors...
    for (int i = initalvalue; i < TotalMicrosteps + 1; i += incrementer)
    {
        if ((i % MicrostepMotor1) == 0)
            Motor1->Rotate(Dir1, 1, (int)deltaTime);
        if ((i % MicrostepMotor2) == 0)
            Motor2->Rotate(Dir2, 1, (int)deltaTime);
    }
}
//Control three stepper motors simultainously with a specified speed and direction.
//Controlling three or more motors requires some complex microstepping alogrithms.
void StepMotors(StepperMotor *Motor1, long Steps1, StepperMotor *Motor2, long Steps2, StepperMotor *Motor3, long Steps3, int Speed)
{
	long TotalMicrosteps = 0; //Total microsteps in the series.
	long MicrostepMotor1 = 0;
	long MicrostepMotor2 = 0;
	long MicrostepMotor3 = 0;

	StepperMotor::MotorDirection Dir1 = GetDirection(Steps1);
	StepperMotor::MotorDirection Dir2 = GetDirection(Steps2);
    StepperMotor::MotorDirection Dir3 = GetDirection(Steps3);

    Steps1 = qAbs(Steps1);
    Steps2 = qAbs(Steps2);
    Steps3 = qAbs(Steps3);

	//Calculate Microstepping
	if (Steps1 == 0)
	{
		TotalMicrosteps = GetLCM(Steps2, Steps3);
		MicrostepMotor2 = TotalMicrosteps / Steps2;
		MicrostepMotor3 = TotalMicrosteps / Steps3;
		MicrostepMotor1 = TotalMicrosteps * 10;
	}
	else if (Steps2 == 0)
	{
		TotalMicrosteps = GetLCM(Steps1, Steps3);
		MicrostepMotor1 = TotalMicrosteps / Steps1;
		MicrostepMotor3 = TotalMicrosteps / Steps3;
		MicrostepMotor2 = TotalMicrosteps * 10;
	}
	else
	{
		//If we're stepping two of them we need to find the 
		//least common multiple of the steps. We'll then iterate through
		//that value looking for the modulus values that yield 0.
		TotalMicrosteps = GetLCM(Steps1, Steps2, Steps3);
		MicrostepMotor1 = TotalMicrosteps / Steps1;
		MicrostepMotor2 = TotalMicrosteps / Steps2;
		MicrostepMotor3 = TotalMicrosteps / Steps3;
	}

	//Total Time
     float Time = qSqrt(qPow(Steps1, 2) + qPow(Steps2, 2)) / Speed;
	//Time delay per step
    float deltaTime = Time / ((float)Steps1 + (float)Steps2);

	int incrementer = GetGCD(Steps1, Steps2, Steps3);
    int initalvalue = qMin(Steps1,qMin(Steps2, Steps3));

	//Rotate Motors...
    for (int i = initalvalue; i < TotalMicrosteps + 1; i += incrementer)
	{
		if ((i % MicrostepMotor1) == 0)
            Motor1->Rotate(Dir1, 1, (int)deltaTime);
		if ((i % MicrostepMotor2) == 0)
            Motor2->Rotate(Dir2, 1, (int)deltaTime);
		if ((i % MicrostepMotor3) == 0)
            Motor3->Rotate(Dir3, 1, (int)deltaTime);
	}
}
