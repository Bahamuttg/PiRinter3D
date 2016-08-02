#ifndef GCODEINTERPRETER_H
#define GCODEINTERPRETER_H
#include "motorcontroller.h"
#include "steppermotor.h"
#include <Qtcore>

class GCodeInterpreter : public QObject
{
private:
	//Define Stepper Motors.
	StepperMotor *XMotor;
	StepperMotor *YMotor;
	StepperMotor *ZMotor;
	StepperMotor *EXMotor;

	/*
	resolution in x direction.Unit: mm  http ://prusaprinters.org/calculator/
	resolution in y direction.Unit: mm  http ://prusaprinters.org/calculator/
	resolution in Z direction.Unit: mm  http ://prusaprinters.org/calculator/
	resolution for Extruder Unit : mm http ://forums.reprap.org/read.php?1,144245
	float Xres, Yres, Zres, EXres;
	*/

	float EngravingSpeed = .20;

	int ExtruderTemp = 0, BedTemp =0;

	//TODO EndStops
	/*
	EndStopX = 14
	EndStopY = 15
	EndStopZ = 7
	ExtHeater = 10
	HeatBed = 9
	ExtThermistor = 11
	HeatBedThermistor = 8
	*/

public:
	GCodeInterpreter();
	~GCodeInterpreter();
};

#endif // GCODEINTERPRETER_H
