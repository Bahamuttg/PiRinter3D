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

struct Coordinate
{
    float value;
    QString Name;
};

class GCodeInterpreter : public QObject
{
private:
    Q_OBJECT

    //Define Stepper Motors.
    StepperMotor *_XMotor, *_YMotor, *_ZMotor, *_ExtMotor;
    ThermalProbe *_BedProbe, *_ExtProbe;
    EndStop *_XStop, *_YStop, *_ZStop;

    //Get Resolutions from the main ui configuration.
    float _XRes, _YRes, _ZRes, _ExtRes;

    //Get speed factor from main ui config
    float _SpeedFactor;

    //Extruder and Bed Temp values
    int _ExtruderTemp, _BedTemp;

    bool TerminateThread;

    void WriteToLogFile(const QString &LogFilePath);

    void ParseLine();

    void HomeAllAxis();

    QList<Coordinate>  GetCoordValues(QString &);

    void MoveToolHead(const float &XPosition, const float &YPosition, const float &ZPosition, const float &ExtPosition);

public:
    bool Stop;

    GCodeInterpreter(const QString &FilePath, const QString &Logpath,
                     StepperMotor *XMotor, StepperMotor *YMotor, StepperMotor *ZMotor, StepperMotor *ExtMotor,
                     ThermalProbe *BedProbe, ThermalProbe *ExtruderProbe);
    ~GCodeInterpreter();

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

    void BeginLineProcessing(QString &);

    void ProcessingMoves(QString &);

    void ProcessingTemps(QString &);

    void EndLineProcessing(QString &);

    void OnError(QString &);

    void OnSuccess();

};

#endif // GCODEINTERPRETER_H
