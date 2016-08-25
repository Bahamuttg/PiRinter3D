#ifndef GCODEINTERPRETER_H
#define GCODEINTERPRETER_H
#include "motorcontroller.h"
#include "steppermotor.h"
#include "thermalprobe.h"
#include "endstop.h"
#include "probeworker.h"
#include <QtCore>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QObject>

struct Coordinate
{
    float value;
    QString Name;
};

class GCodeInterpreter : public QObject
{
private:
    Q_OBJECT

    MotorController _Controller;
    QStringList _GCODE;

    //Define Stepper Motors.
    StepperMotor *_XAxis, *_YAxis, *_ZAxis, *_ExtAxis;

    EndStop *_XStop, *_YStop, *_ZStop;

    QThread *_ExtruderThread, *_BedThread;
	
    //Get Resolutions from the main ui configuration.
    float _XRes, _YRes, _ZRes, _ExtRes;

    //Get speed factor from main ui config
    float _SpeedFactor;

    //Extruder and Bed Temp values
    int _ExtruderTemp, _BedTemp;

    bool _TerminateThread, _Stop;

    void InitializeMotors();
    void InitializeThermalProbes();
    void InitializeEndStops();
	
    void WriteToLogFile(const QString &);
    void ParseLine(QString &);
    void HomeAllAxis();
	void ExecutePrintSequence();
    QList<Coordinate>  GetCoordValues(QString &);

    void MoveToolHead(const float &XPosition, const float &YPosition, const float &ZPosition, const float &ExtPosition);

public:
    GCodeInterpreter(const QString &FilePath, const QString &Logpath);
    ~GCodeInterpreter();

    ProbeWorker *BedProbe, *ExtProbe;

    void LoadGCode(const QString &FilePath);

    int GetExtruderTemp();
    void SetExtruderTemp(const int &);

    int GetBedTemp();
    void SetBedTemp(const int &);

public slots:
    void BeginPrint();

    void PausePrint();

    void TerminatePrint();

signals:
    void PrintStarted();

    void PrintComplete();

    void BeginLineProcessing(QString);
	
    void EndLineProcessing(QString);

    void ProcessingMoves(QString);
	
    void MoveComplete(QString);

    void ProcessingTemps(QString);

    void TemperatureAtTarget(int);
	
    void TemperatureHigh(int);
	
    void TemperatureLow(int);

    void OnError(QString);

    void OnSuccess();
	
    void ReportProgress(int);
};

#endif // GCODEINTERPRETER_H
