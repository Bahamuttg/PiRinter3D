#include "steppermotor.h"
#include <wiringPi.h>
#include <string>

StepperMotor::StepperMotor(int Coil1, int Coil3, bool IsHalfStep, string Name)
{
    this->MotorName = Name;
    this->_Coil_1 = Coil1;
    this->_Coil_2 = 0;
    this->_Coil_3 = Coil3;
    this->_Coil_4 = 0;
    this->_IsNOTGated = true;
    this->_IsHalfStep = IsHalfStep;
    this->_Enabled = true;
    this->_IsInverted = false;
    this->_IsRotating = false;

    //Setup Pin->Coil Mappings
    pinMode(_Coil_1, OUTPUT);
	pinMode(_Coil_3, OUTPUT);
	
    this->_Phase = 0;
    this->Direction = CLOCKWISE;
    this->Position = 0;
    this->HoldPosition = true;
}

StepperMotor::StepperMotor(int Coil1, int Coil2, int Coil3, int Coil4, bool IsHalfStep, string Name)
{
    this->MotorName = Name;
	this->_Coil_1 = Coil1;
	this->_Coil_2 = Coil2;
	this->_Coil_3 = Coil3;
	this->_Coil_4 = Coil4;
	this->_IsNOTGated = false;
	this->_IsHalfStep = IsHalfStep;
	this->_Enabled = true;
	this->_IsInverted = false;
	this->_IsRotating = false;

	//Setup Pin->Coil Mappings
	pinMode(_Coil_1, OUTPUT);
	pinMode(_Coil_2, OUTPUT);
	pinMode(_Coil_3, OUTPUT);
	pinMode(_Coil_4, OUTPUT);

    this->_Phase = 0;
	this->Direction = CLOCKWISE;
	this->Position = 0;
	this->HoldPosition = true;
}

StepperMotor::~StepperMotor()
{
	CoilsOff();
}

void StepperMotor::Rotate(MotorDirection Direction,  long Steps, int MS_Delay)
{
    if (_Enabled)
        if(Steps < 0)
            Steps *= -1;
     //this is the minimum speed at which we can switch the coils.
    if(MS_Delay < 2)
        MS_Delay = 2;
        for (int i = 0; i < Steps; i++)
		{
			PerformStep(Direction);
			delay(MS_Delay);
		}
}

void StepperMotor::Rotate(MotorDirection Direction, int MS_Delay)
{
    //this is the minimum speed at which we can switch the coils.
    if(MS_Delay < 2)
        MS_Delay = 2;
	//This method will block indefinitely.
	//It's up to the programmer to thread it properly.
	while (_Enabled)
	{
		PerformStep(Direction);
		delay(MS_Delay);
	}
}

void StepperMotor::StopRotation(const bool &Hold)
{
	this->_Enabled = false;
	if (!Hold)
		CoilsOff();
}

void StepperMotor::PerformStep(MotorDirection Direction)
{
	//full step sequence. maximum torque
	const unsigned int FullStep[4][4] = { {1,0,1,0},{0,1,1,0},{0,1,0,1},{1,0,0,1} };
	//half-step sequence. double resolution. But the torque of the stepper motor is not constant
	const unsigned int HalfStep[8][4] = { {1,0,0,0},{1,0,1,0},{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,1,0,1},{0,0,0,1},{1,0,0,1} };

	int TargetPhase;
	this->Direction = Direction;

	if (_IsInverted)
		InvertDirection();
	if (_Enabled)
	{
		//Record our target phase
        TargetPhase = (this->_Phase + this->Direction);

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
			digitalWrite(_Coil_1, FullStep[TargetPhase][0]);
			digitalWrite(_Coil_3, FullStep[TargetPhase][2]);
			if (!_IsNOTGated)
			{
				digitalWrite(_Coil_2, FullStep[TargetPhase][1]);
				digitalWrite(_Coil_4, FullStep[TargetPhase][3]);
			}
		}
		else
		{
			digitalWrite(_Coil_1, HalfStep[TargetPhase][0]);
			digitalWrite(_Coil_3, HalfStep[TargetPhase][2]);
			if (!_IsNOTGated)
			{
				digitalWrite(_Coil_2, HalfStep[TargetPhase][1]);
				digitalWrite(_Coil_4, HalfStep[TargetPhase][3]);
			}
		}

		//Update motor status.
        this->_Phase = TargetPhase;
		this->Position += this->Direction;
	}
	//If we're not holding the motor position let's release it.
	if (!HoldPosition)
		CoilsOff();
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
void StepperMotor::SetInverted(const bool &Arg)
{
        this->_IsInverted = Arg;
}

void StepperMotor::CoilsOff()
{
    digitalWrite(_Coil_1, LOW);
	digitalWrite(_Coil_2, LOW);
	digitalWrite(_Coil_3, LOW);
	digitalWrite(_Coil_4, LOW);
}

void StepperMotor::InvertDirection()
{
	if (this->Direction == CLOCKWISE)
		this->Direction = CTRCLOCKWISE;
	else
		this->Direction = CLOCKWISE;
	this->_IsInverted = true;
}
 void StepperMotor::SetNotGated(const bool &Arg)
 {
     this->_IsNOTGated = Arg;
 }
