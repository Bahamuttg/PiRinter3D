/*
* =================BEGIN GPL LICENSE BLOCK=========================================
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
#ifndef GCODEINTERPRETER_H
#define GCODEINTERPRETER_H
#include "motorcontroller.h"
#include "steppermotor.h"
#include "thermalprobe.h"
#include "endstop.h"
#include "probeworker.h"
#include "adccontroller.h"
#include <QtCore>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QObject>
#include <QThread>

struct Coordinate
{
    float value;
    QString Name;
};

class GCodeInterpreter : public QThread
{
private:
    Q_OBJECT

    MotorController _Controller;

    QStringList _GCODE;

    //Define Stepper Motors.
    StepperMotor *_XAxis, *_YAxis, *_ZAxis, *_ExtAxis;

    EndStop *_XStop, *_YStop, *_ZStop;

    ThermalProbe *_BedProbe, *_ExtProbe;
	
    ADCController *_ADCController;
    //Get Resolutions from the main ui configuration.
    float _XRes, _YRes, _ZRes, _ExtRes;

    //Get speed factor from main ui config
    float _SpeedFactor;

    //Extruder and Bed Temp values
    int _ExtruderTemp, _BedTemp;

    //Print dimensions
    int _XArea, _YArea;

    //TODO: Add ofsets for multiple tool heads.

    bool _TerminateThread, _Stop, _IsPrinting;

    void InitializeMotors();
    void InitializeThermalProbes();
    void InitializeEndStops();
    void InitializeADCConverter();

    void WriteToLogFile(const QString &);
    void ParseLine(QString &GString);
    void HomeAllAxis();
	void ExecutePrintSequence();
    QList<Coordinate>  GetCoordValues(QString &GString);

    void MoveToolHead(const float &XPosition, const float &YPosition, const float &ZPosition, const float &ExtPosition);

protected:
    void run();

public:
    GCodeInterpreter(const QString &FilePath, const int &XArea, const int & YArea, QObject * parent = 0);
    ~GCodeInterpreter();

    ProbeWorker *BedProbeWorker, *ExtProbeWorker;

    void LoadGCode(const QString &FilePath);

    int GetTotalLines()
    {
        return _GCODE.length();
    }

    int GetExtruderTemp()
    {
        return this->ExtProbeWorker->TriggerProbeRead();
    }
    void SetExtruderTemp(const int &CelsiusValue)
    {
        this->_ExtruderTemp = CelsiusValue;
        this->ExtProbeWorker->SetTargetTemp(CelsiusValue);
    }

    int GetBedTemp()
    {
        return this->BedProbeWorker->TriggerProbeRead();
    }
    void SetBedTemp(const int &CelsiusValue)
    {
        this->_BedTemp = CelsiusValue;
        this->BedProbeWorker->SetTargetTemp(CelsiusValue);
    }

    bool IsPrinting()
    {
        return _IsPrinting;
    }
    bool IsPaused()
    {
        return _Stop;
    }

    bool IsFinished()
    {
        return _TerminateThread;
    }

public slots:
    void BeginPrint();

    void PausePrint();

    void TerminatePrint();

    void ChangeBedTemp(const int &Celsius);

    void ChangeExtTemp(const int &Celsius);

signals:
    void PrintStarted();

    void PrintComplete();

    void BeginLineProcessing(QString);
	
    void EndLineProcessing(QString);

    void ProcessingMoves(QString);
	
    void MoveComplete(QString);

    void ProcessingTemps(QString);

    void BedTemperatureChanged(int);

    void ExtruderTemperatureChanged(int);

    void TemperatureAtTarget(int);
	
    void TemperatureHigh(int);
	
    void TemperatureLow(int);

    void OnError(QString);

    void OnSuccess();
	
    void ReportProgress(int);
};

#endif // GCODEINTERPRETER_H
