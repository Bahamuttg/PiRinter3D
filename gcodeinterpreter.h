#ifndef GCODEINTERPRETER_H
#define GCODEINTERPRETER_H
#include "motorcontroller.h"
#include "steppermotor.h"
#include <QtCore>
#include <QFile>
#include <QTextStream>
#include <QList>

class GCodeInterpreter : public QObject
{
private:
	//Define Stepper Motors.
    StepperMotor *_XMotor;
    StepperMotor *_YMotor;
    StepperMotor *_ZMotor;
    StepperMotor *_EXMotor;

    //Get Resolutions from the main ui configuration.
	/*
	resolution in x direction.Unit: mm  http ://prusaprinters.org/calculator/
	resolution in y direction.Unit: mm  http ://prusaprinters.org/calculator/
	resolution in Z direction.Unit: mm  http ://prusaprinters.org/calculator/
	resolution for Extruder Unit : mm http ://forums.reprap.org/read.php?1,144245
	float Xres, Yres, Zres, EXres;
	*/

    //Get Engraving speed from main ui config
    float _EngravingSpeed = .20;

    int _ExtruderTemp = 0, _BedTemp =0;

    //TODO  get EndStops
	/*
	EndStopX = 14
	EndStopY = 15
	EndStopZ = 7
	ExtHeater = 10
	HeatBed = 9
	ExtThermistor = 11
	HeatBedThermistor = 8
	*/
void WriteToLogFile(const QString &LogFilePath);

public:
    GCodeInterpreter(const QString &FilePath, const QString &Logpath,
                     StepperMotor *XMotor, StepperMotor *YMotor, StepperMotor *ZMotor, StepperMotor *EXMotor);
	~GCodeInterpreter();
    void BeginPrint();
};

#endif // GCODEINTERPRETER_H
