#ifndef GCODEINTERPRETER_H
#define GCODEINTERPRETER_H
#include "motorcontroller.h"
#include "steppermotor.h"
#include "thermalprobe.h"
#include "endstop.h"
#include <QtCore>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QObject>

class GCodeInterpreter : public QObject
{
private:
    Q_OBJECT

        //Define Stepper Motors.
        StepperMotor *_XMotor, *_YMotor, *_ZMotor, *_ExtMotor;
        ThermalProbe *_BedProbe, *_ExtProbe;
        EndStop *_XStop, *_YStop, *_ZStop;

        //Get Resolutions from the main ui configuration.
        float _XRes, _YRes, _ZRes, ExtRes;

        //Get Engraving speed from main ui config
        float _EngravingSpeed;
        //Extruder and Bed Temp values
        int _ExtruderTemp, _BedTemp;


    void WriteToLogFile(const QString &LogFilePath);

    void ParseLine();

public:
    GCodeInterpreter(const QString &FilePath, const QString &Logpath,
                     StepperMotor *XMotor, StepperMotor *YMotor, StepperMotor *ZMotor, StepperMotor *ExtMotor,
                     ThermalProbe *BedProbe, ThermalProbe *ExtruderProbe);
	~GCodeInterpreter();
    void BeginPrint();

signals:
        void BeginLineProcessing(QString &);

        void ProcessingMoves(QString &);

        void ProcessingTemps(QString &);

        void EndLineProcessing(QString &);

        void OnError(QString &);

        void OnSuccess();

};

#endif // GCODEINTERPRETER_H
