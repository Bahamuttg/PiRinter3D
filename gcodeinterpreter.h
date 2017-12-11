/*
* =================BEGIN GPL LICENSE BLOCK=========================================
*  Author: Thomas George
*  PiRinter3D Copyright (C) 2016
*
*  This program is free software; you can redistribute it and/or 
*  modify it under the terms of the GNU General Public License 
*  as published by the Free Software Foundation; either version 3 
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
#include "probeworker.h"
#include "adccontroller.h"
#include "fan.h"
#include <QtCore>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QObject>
#include <QThread>
#include <QList>
#include <QTime>

struct Coordinate
{
    float value;
    QString Name;
};

class GCodeInterpreter : public QThread
{
private:
    Q_OBJECT

    enum ArcDirection
    {
        CLOCKWISE = 2,
        CTRCLOCKWISE = 3
    };

    //GCODE entries
    QStringList _GCODE;

    //List of Z Axis only moves. For 3D prints only. Disable for CNC mode.
    QList<int> _Layers;
    //TODO: ZAxis tracking for Print recovery. Possibly a QList  of Z Move indexes so we can reset the loop counter.
    //Home motors and/or offer up dialog for head positioning then restart layer when triggered.

    //Define integral components
    //These could be arrays for future expnsion of  quantities.
    StepperMotor *_XAxis, *_YAxis, *_ZAxis, *_ExtAxis;
    Fan *_Fan1;
    ThermalProbe *_BedProbe, *_ExtProbe;
    ADCController *_ADCController;
    MotorController *_Controller;

    //Get Resolutions from the main ui configuration.
    float _XRes, _YRes, _ZRes, _ExtRes;

    //Get speed factor from main ui config
    float _SpeedFactor;
    float _SpeedActual;
    float _SpeedModulator;

    //Extruder and Bed target temperature values from GCODE
    int _ExtruderTemp, _BedTemp;

    //Print dimensions
    int _XArea, _YArea, _ZArea;

    //TODO: Add offsets for multiple tool heads.

    bool _TerminateThread, _Stop, _IsPrinting;

    void InitializeMotors();
    void InitializeFans();
    void InitializeThermalProbes();
    void InitializePrintArea();
    void InitializeADCConverter();

    void WriteToLogFile(const QString &);
    void ParseLine(QString &GString);
    void HomeAllAxis();
    void ExecutePrintSequence();
    QList<Coordinate>  GetCoordValues(QString &GString);

    void MoveToolHead(const float &XPosition, const float &YPosition, const float &ZPosition, const float &ExtPosition);
    void ExecuteArcMove(const float &XPosition, const float &YPosition, const float &ZPosition, const float &ExtPosition, const float &IValue, const float &JValue, ArcDirection Direction);

protected:
    void run();

public:
    GCodeInterpreter(const QString &FilePath, QObject * parent = 0);
    ~GCodeInterpreter();

    QString EstimatedTime;
    QDateTime StartTime;
    QTime ElapsedTime;

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
        this->ExtProbeWorker->SetTargetTemp(CelsiusValue);
    }

    int GetBedTemp()
    {
        return this->BedProbeWorker->TriggerProbeRead();
    }

    void SetBedTemp(const int &CelsiusValue)
    {
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

    int GetBedGcodeTemp()
    {
        return _BedTemp;
    }

    int GetExtGcodeTemp()
    {
        return _ExtruderTemp;
    }

    int GetFanRPM()
    {
        return _Fan1->GetFanRPM();
    }

    int GetFanDuty()
    {
        return _Fan1->GetDutyCycle();
    }

public slots:
    void BeginPrint();

    void PausePrint();

    void TerminatePrint();

    void ChangeBedTemp(const int &Celsius);

    void ChangeExtTemp(const int &Celsius);

    void ChangeFanDuty(const int &DutyCycle);

    void UpdatePositionLabel(QString Name, const long Pos);

    void ModulateSpeed(const int &Factor);

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

    void FanDutyChanged(int);

    void TemperatureAtTarget(int);

    void TemperatureHigh(int);

    void TemperatureLow(int);

    void OnError(QString Val1, QString Val2 = "");

    void OnSuccess();

    void ReportProgress(int);

    void ReportMotorPosition(QString Name, const long Pos);

    void ReportElapsedTime(QString Time);
};

#endif // GCODEINTERPRETER_H
