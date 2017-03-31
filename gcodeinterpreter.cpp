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
#include "gcodeinterpreter.h"
#include "thermalprobe.h"
#include "steppermotor.h"
#include "motorcontroller.h"
#include <QMessageBox>

GCodeInterpreter::GCodeInterpreter(const QString &FilePath, QObject *parent)
    :QThread(parent)
{
    //===========Objects================
    _XAxis = 0;
    _YAxis = 0;
    _ZAxis = 0;
    _ExtAxis = 0;
    _Controller = new MotorController();

    _ExtProbe = 0;
    _BedProbe = 0;
    BedProbeWorker = 0;
    ExtProbeWorker = 0;
    //=========== END Objects================

    _ExtRes = 0;
    _XRes = 0;
    _YRes = 0;
    _ZRes = 0;

    _SpeedFactor = .15;

    _IsPrinting = false;
    _Stop = false;
    _TerminateThread = false;

    InitializeMotors();
    InitializePrintArea();
    InitializeThermalProbes();
    LoadGCode(FilePath);
}

GCodeInterpreter::~GCodeInterpreter()
{
    delete _XAxis;
    delete _YAxis;
    delete _ZAxis;
    delete _ExtAxis;
    delete _ExtProbe;
    delete _BedProbe;
    delete _Controller;

    BedProbeWorker->Terminate();
    ExtProbeWorker->Terminate();
    ExtProbeWorker->wait();
    BedProbeWorker->wait();
    delete BedProbeWorker;
    delete ExtProbeWorker;
    delete _ADCController;
}

void GCodeInterpreter::run()
{
    BeginPrint();
}
//======================================Private Methods============================================
void GCodeInterpreter::LoadGCode(const QString &FilePath)
{
    bool IsTooBig = false;
    QFile PrintFile(FilePath);
    if(PrintFile.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        QTextStream Stream(&PrintFile);
        while (!Stream.atEnd())
        {
            //float LastX = 0, lastY = 0;
            //long TotalMS = 0;

            QString Line = Stream.readLine();
            if(!Line.startsWith(";") && !Line.isEmpty())//Weed out the comment lines and empties.
            {
                _GCODE << Line.split(";")[0];//Again weed out any comments, we want just raw GCODE.

                //Validate the GCODE and make sure the print isn't too big to fit on the bed.
                if (Line.split(" ")[0].toUpper().startsWith("G") && !IsTooBig)
                {
                    QList<Coordinate> Coords = GetCoordValues(Line);
                    float XVal = 0, YVal = 0;
                    for(int i =0; i < Coords.length(); i++)
                    {
                        if(Coords[i].Name == "XAxis")
                            XVal = Coords[i].value;
                        if(Coords[i].Name == "YAxis")
                            YVal = Coords[i].value;
                        if(Coords[i].Name == "ZAxis")
                            _Layers.append(_GCODE.length() - 1);//GCODE Line indexes for layers.
                    }
                    if(XVal > _XArea || YVal > _YArea)
                        IsTooBig = true;
                }
            }
        }
        PrintFile.close();
        if(IsTooBig)
            emit OnError("This print has a print area that is larger than the print area defined in the settings!");
    }
}

void GCodeInterpreter::InitializeADCConverter()
{
    QFile ProbeConfig("ProbeCfg.ini");
    if(ProbeConfig.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        QTextStream CfgStream(&ProbeConfig); //load config file
        while (!CfgStream.atEnd())
        {
            QString Line = CfgStream.readLine(); //read one line at a time
            if(Line.contains("ADCConfig"))
            {
                QStringList Params = Line.split(";");
                if(Params[0].contains("ADC1"))
                {
                    _ADCController = new ADCController(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(), this);
                }
            }
        }
        ProbeConfig.close();
    }
    else
    {
        _ADCController = 0;
        QMessageBox::critical(0, "Error Opening Print Area Config File!", ProbeConfig.errorString(), QMessageBox::Ok);
        emit OnError(ProbeConfig.errorString());
        this->TerminatePrint();
    }
}

void GCodeInterpreter::InitializePrintArea()
{
    QFile AreaCfg("AreaCfg.ini");
    if(AreaCfg.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        QTextStream CfgStream(&AreaCfg); //load config file
        while (!CfgStream.atEnd())
        {
            QString Line = CfgStream.readLine(); //read one line at a time
            if(Line.contains("PrintAreaCfg"))
            {
                QStringList Params = Line.split(";");
                if(Params[0].contains("XArea"))
                    _XArea = Params[1].toInt();
                if(Params[0].contains("YArea"))
                    _YArea = Params[1].toInt();
            }
        }
        AreaCfg.close();
    }
    else
    {
        QMessageBox::critical(0, "Error Opening Print Area Config File!", AreaCfg.errorString(), QMessageBox::Ok);
        emit OnError(AreaCfg.errorString());
        this->TerminatePrint();
    }
}

void GCodeInterpreter::InitializeThermalProbes()
{
    InitializeADCConverter();

    if(_ADCController == 0)
        return;

    QFile ProbeConfig("ProbeCfg.ini");
    if(ProbeConfig.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        QTextStream CfgStream(&ProbeConfig); //load config file
        while (!CfgStream.atEnd())
        {
            QString Line = CfgStream.readLine(); //read one line at a time
            if(Line.contains("ProbeCfg"))
            {
                QStringList Params = Line.split(";");
                if(Params[0].contains("Extruder"))
                {
                    _ExtProbe = new ThermalProbe(Params[1].toDouble(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(),
                            Params[5].toDouble(), Params[6].toInt(),  Params[7].toInt(), _ADCController);
                    ExtProbeWorker = new ProbeWorker(_ExtProbe, Params[8].toInt(), this);
                    connect(this, SIGNAL(terminated()), ExtProbeWorker, SLOT(terminate()));
                }
                if(Params[0].contains("Bed"))
                {
                    _BedProbe = new ThermalProbe(Params[1].toDouble(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(),
                            Params[5].toDouble(), Params[6].toInt(),  Params[7].toInt(), _ADCController);
                    BedProbeWorker = new ProbeWorker(_BedProbe, Params[8].toInt(), this);
                    connect(this, SIGNAL(terminated()), BedProbeWorker, SLOT(terminate()));
                }
            }
        }
        ProbeConfig.close();
    }
    else
    {
        QMessageBox::critical(0, "Error Opening Probe Config File!", ProbeConfig.errorString(), QMessageBox::Ok);
        emit OnError(ProbeConfig.errorString());
        this->TerminatePrint();
    }
}

void GCodeInterpreter::InitializeMotors()
{
    QFile MotorConfig("MotorCfg.ini");
    if(MotorConfig.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        QTextStream CfgStream(&MotorConfig); //load config file
        while (!CfgStream.atEnd())
        {
            QString Line = CfgStream.readLine(); //read one line at a time
            if(Line.contains("MotorConfig"))
            {
                QStringList Params = Line.split(";");
                if(Params[0].contains("XAxis"))
                {
                    if(Params[6].toInt()) //If it's using HEX inverters
                        this->_XAxis = new StepperMotor(Params[1].toInt(), Params[3].toInt(), Params[11].toInt(), Params[0].split("::")[1].toStdString(), Params[5].toInt());
                    else if(Params[16].toInt())
                        this->_XAxis = new StepperMotor(Params[13].toInt(), Params[14].toInt(), Params[15].toInt(), Params[11].toInt(),  Params[0].split("::")[1].toStdString(), Params[5].toInt());
                    else
                        this->_XAxis = new StepperMotor(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(), Params[11].toInt(),
                            Params[9].toInt(), Params[0].split("::")[1].toStdString(), Params[5].toInt());
                    this->_XRes = Params[10].toFloat();
                    this->_XAxis->SetHoldOnIdle(Params[17].toInt());
                }
                if(Params[0].contains("YAxis"))
                {
                    if(Params[6].toInt())
                        this->_YAxis = new StepperMotor(Params[1].toInt(), Params[3].toInt(), Params[11].toInt(), Params[0].split("::")[1].toStdString(), Params[5].toInt());
                    else if(Params[16].toInt())
                        this->_YAxis = new StepperMotor(Params[13].toInt(), Params[14].toInt(), Params[15].toInt(), Params[11].toInt(),  Params[0].split("::")[1].toStdString(), Params[5].toInt());
                    else
                        this->_YAxis = new StepperMotor(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(), Params[11].toInt(),
                            Params[9].toInt(), Params[0].split("::")[1].toStdString(), Params[5].toInt());
                    this->_YRes = Params[10].toFloat();
                    this->_YAxis->SetHoldOnIdle(Params[17].toInt());
                }
                if(Params[0].contains("ZAxis"))
                {
                    if(Params[6].toInt())
                        this->_ZAxis = new StepperMotor(Params[1].toInt(), Params[3].toInt(), Params[11].toInt(), Params[0].split("::")[1].toStdString(), Params[5].toInt());
                    else if(Params[16].toInt())
                        this->_ZAxis = new StepperMotor(Params[13].toInt(), Params[14].toInt(), Params[15].toInt(), Params[11].toInt(),  Params[0].split("::")[1].toStdString(), Params[5].toInt());
                    else
                        this->_ZAxis = new StepperMotor(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(), Params[11].toInt(),
                            Params[9].toInt(), Params[0].split("::")[1].toStdString(), Params[5].toInt());
                    this->_ZRes = Params[10].toFloat();
                    this->_ZAxis->SetHoldOnIdle(Params[17].toInt());
                }
                if(Params[0].contains("ExtAxis"))
                {
                    if(Params[6].toInt())
                        this->_ExtAxis = new StepperMotor(Params[1].toInt(), Params[3].toInt(), Params[11].toInt(), Params[0].split("::")[1].toStdString(), Params[5].toInt());
                    else if(Params[16].toInt())
                        this->_ExtAxis = new StepperMotor(Params[13].toInt(), Params[14].toInt(), Params[15].toInt(), Params[11].toInt(),  Params[0].split("::")[1].toStdString(), Params[5].toInt());
                    else
                        this->_ExtAxis = new StepperMotor(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(), Params[11].toInt(),
                            Params[9].toInt(), Params[0].split("::")[1].toStdString(), Params[5].toInt());
                    this->_ExtRes = Params[10].toFloat();
                    this->_ExtAxis->SetHoldOnIdle(Params[17].toInt());
                }
            }
        }
        MotorConfig.close();
    }
    else
    {
        QMessageBox::critical(0, "Error Opening Motor Config File!", MotorConfig.errorString(), QMessageBox::Ok);
        emit OnError(MotorConfig.errorString());
        this->TerminatePrint();
    }
}

void GCodeInterpreter::HomeAllAxis()
{
    bool Done;
    if(_XAxis->IsAgainstStop() || _YAxis->IsAgainstStop() || _ZAxis->IsAgainstStop() || _ExtAxis->IsAgainstStop())
    {
        QMessageBox::critical(0, "Ambigious End Stop Detected!", "An end stop on one of the axis is triggered and PiRinter3D cannot determine which one it is. Please check the machine configuration and manually move the carriages from the end stops to continue!", QMessageBox::Ok);
        this->TerminatePrint();
    }
    do
    {
        /*	TODO: we need to configure this method to be able to handle moving the motor from the near and far end stop
     *	if it is already on them when we enter this method. Case: when the motor is instantiated it is set to a default
     *	rotation of CLOCKWISE. If it is already against the CTRCLOCKWISE stop it will bash the carriage against the cradle
     * until the user intervenes (NOT GOOD)! On the other end of the spectrum... If it is already against the CLOCKWISE stop,
     *	we will just end up moving the motor off the stop on that side. This will render the carriage it is on useless because
     *	it will be just off of the far stop and quickly hit it again when we try to process GCODE.
     *
     */
        Done = true;
        if(_XAxis->HasEndstop() && !_XAxis->IsAgainstStop())
        {
            _XAxis->Rotate(StepperMotor::CTRCLOCKWISE, 1, _XAxis->MaxSpeed());
            Done = false;
        }

        if(_YAxis->HasEndstop() && !_YAxis->IsAgainstStop())
        {
            _YAxis->Rotate(StepperMotor::CTRCLOCKWISE, 1, _YAxis->MaxSpeed());
            Done = false;
        }

        if(_ZAxis->HasEndstop() && !_ZAxis->IsAgainstStop())
        {
            _ZAxis->Rotate(StepperMotor::CTRCLOCKWISE, 1, _ZAxis->MaxSpeed());
            Done = false;
        }

        if(_ExtAxis->HasEndstop() && !_ExtAxis->IsAgainstStop())
        {
            _ExtAxis->Rotate(StepperMotor::CTRCLOCKWISE, 1, _ExtAxis->MaxSpeed());
            Done = false;
        }
    } while(!Done && !_TerminateThread);
    //Get them off of the stops.
    while(!_XAxis->MoveFromEndstop() && !_TerminateThread);
    while(!_YAxis->MoveFromEndstop() && !_TerminateThread);
    while(!_ZAxis->MoveFromEndstop() && !_TerminateThread);
    while(!_ExtAxis->MoveFromEndstop() && !_TerminateThread);
    //Buffer X and Y phases
    _XAxis->Rotate(StepperMotor::CLOCKWISE, 2, _XAxis->MaxSpeed());
    _YAxis->Rotate(StepperMotor::CLOCKWISE, 2, _XAxis->MaxSpeed());
    //Reset the positions of the motors to the origins.
    _XAxis->Position = 0;
    _YAxis->Position = 0;
    _ZAxis->Position = 0;
    _ExtAxis->Position = 0;
    emit MoveComplete("Homing Complete");
}

void GCodeInterpreter::MoveToolHead(const float &XPosition, const float &YPosition, const float &ZPosition, const float &ExtPosition)
{
    int stepx = 0, stepy = 0, stepz = 0, stepext = 0;
    if(XPosition != 0)
        stepx = qFloor((XPosition/_XRes) + 0.5) - _XAxis->Position; //this is how I round.
    if(YPosition != 0)
        stepy = qFloor((YPosition/_YRes) + 0.5) - _YAxis->Position;
    if(ZPosition != 0)
        stepz = qFloor((ZPosition/_ZRes) + 0.5) - _ZAxis->Position;
    if(ExtPosition != 0)
        stepext = qFloor((ExtPosition/_ExtRes) + 0.5) - _ExtAxis->Position;

    float total_steps = qSqrt((qPow(stepx, 2) + qPow(stepy, 2)));
    float total_3dsteps = total_steps * stepz;
    //_SpeedFactor = FCode / 60000 (1500 / 60000 = .025)
    //Resolution = Travel per Revolution / Steps per Revolution (14mm / 200 = .07)
    //MSDelay = 1 / (_SpeedFactor / Resolution) (1 / (.025 / .07) = 2.8)
    //2.8 MS delay between steps to meet F1500 Requirements
    if(total_steps != 0 && total_3dsteps != 0 && stepext != 0)
    {
        _Controller->StepMotors(*_XAxis, stepx, *_YAxis, stepy, *_ZAxis, stepz, *_ExtAxis, stepext,
                               (1 / (_SpeedFactor / qMin(_XRes, qMin(_ZRes, _YRes)))));
        emit ProcessingMoves("Printing....");
    }
    else if(total_steps != 0 && total_3dsteps != 0)
    {
        _Controller->StepMotors(*_XAxis, stepx, *_YAxis, stepy, *_ZAxis, stepz,
                               (1 / (_SpeedFactor / qMin(_XRes, qMin(_ZRes, _YRes)))));
        emit ProcessingMoves("Movind Tool Head 3D....");
    }
    else if(total_steps != 0 && stepext != 0)
    {
        _Controller->StepMotors(*_XAxis, stepx, *_YAxis, stepy, *_ExtAxis, stepext,
                               (1 / (_SpeedFactor / qMin(_XRes, _YRes))));
        emit ProcessingMoves("Printing....");
    }
    else if(total_steps != 0)
    {
        _Controller->StepMotors(*_XAxis, stepx, *_YAxis, stepy,
                               (1 / (_SpeedFactor / qMin(_XRes, _YRes))));
        emit ProcessingMoves("Moving Tool Head....");
    }
    else if (stepz != 0)
    {
        emit ProcessingMoves("Moving Z Axis...");
        _Controller->StepMotor(*_ZAxis, stepz, (1 / (_SpeedFactor / _ZRes)));
    }
    else if (stepext != 0)
    {
        emit ProcessingMoves("Moving Extruder...");
        _Controller->StepMotor(*_ExtAxis, stepext,(1 / (_SpeedFactor / _ExtRes)));
    }
}
void GCodeInterpreter::ExecuteArcMove(const float &XPosition, const float &YPosition, const float &ZPosition, const float &ExtPosition,
                                      const float &IValue, const float &JValue, ArcDirection Direction)
{
    /*
    elif (lines[0:3]=='G02')|(lines[0:3]=='G03'): #circular interpolation
    old_x_pos=x_pos;
    old_y_pos=y_pos;

    ext_pos = 0;

    #still need to add code here to handle extrusion info from the line if it is available
    if(lines.find('E') >= 0):
        #get E value as well as the rest
        [x_pos,y_pos]=XYposition(lines);
        [i_pos,j_pos,ext_pos]=IJEposition(lines);
    else:
        [x_pos,y_pos]=XYposition(lines);
        [i_pos,j_pos]=IJposition(lines);

    xcenter=old_x_pos+i_pos;   #center of the circle for interpolation
    ycenter=old_y_pos+j_pos;


    Dx=x_pos-xcenter;
    Dy=y_pos-ycenter;      #vector [Dx,Dy] points from the circle center to the new position

    r=sqrt(i_pos**2+j_pos**2);   # radius of the circle

    e1=[-i_pos,-j_pos]; #pointing from center to current position
    if (lines[0:3]=='G02'): #clockwise
        e2=[e1[1],-e1[0]];      #perpendicular to e1. e2 and e1 forms x-y system (clockwise)
    else:                   #counterclockwise
        e2=[-e1[1],e1[0]];      #perpendicular to e1. e1 and e2 forms x-y system (counterclockwise)

    #[Dx,Dy]=e1*cos(theta)+e2*sin(theta), theta is the open angle

    costheta=(Dx*e1[0]+Dy*e1[1])/r**2;
    sintheta=(Dx*e2[0]+Dy*e2[1])/r**2;        #theta is the angle spanned by the circular interpolation curve

    if costheta>1:  # there will always be some numerical errors! Make sure abs(costheta)<=1
        costheta=1;
    elif costheta<-1:
        costheta=-1;

    theta=arccos(costheta);
    if sintheta<0:
        theta=2.0*pi-theta;

    no_step=int(round(r*theta/dx/5.0));   # number of point for the circular interpolation
    extruderMovePerStep = 0;
    if ext_pos != 0:
        extruderMovePerStep = (ext_pos - MExt.position)/no_step;

    for i in range(1,no_step+1):
        tmp_theta=i*theta/no_step;
        tmp_x_pos=xcenter+e1[0]*cos(tmp_theta)+e2[0]*sin(tmp_theta);
        tmp_y_pos=ycenter+e1[1]*cos(tmp_theta)+e2[1]*sin(tmp_theta);
        if extruderMovePerStep == 0:
            moveto(MX,tmp_x_pos,dx,MY, tmp_y_pos,dy,speed,True);
        else:
            movetothree(MX,tmp_x_pos,dx,MY, tmp_y_pos,dy,MExt,MExt.position+extruderMovePerStep,dext,speed,True);
         */
}

QList<Coordinate>  GCodeInterpreter::GetCoordValues(QString &GString)
{
    QList<Coordinate> Coords;
    if(GString.length() > 0)
    {
        QStringList CoordList = GString.toUpper().split(" ");
        for(int i = 0; i < CoordList.length(); i++)
        {
            Coordinate Coord;
            if(CoordList[i].contains("X"))
            {
                Coord.Name = "XAxis";
                Coord.value = CoordList[i].mid(1).toFloat();
                Coords.append(Coord);
            }
            if(CoordList[i].contains("Y"))
            {
                Coord.Name = "YAxis";
                Coord.value = CoordList[i].mid(1).toFloat();
                Coords.append(Coord);
            }
            if(CoordList[i].contains("Z"))
            {
                Coord.Name = "ZAxis";
                Coord.value = CoordList[i].mid(1).toFloat();
                Coords.append(Coord);
            }
            if(CoordList[i].contains("E"))
            {
                Coord.Name = "ExtAxis";
                Coord.value = CoordList[i].mid(1).toFloat();
                Coords.append(Coord);
            }
            if(CoordList[i].contains("I"))
            {
                Coord.Name = "IValue";
                Coord.value = CoordList[i].mid(1).toFloat();
                Coords.append(Coord);
            }
            if(CoordList[i].contains("J"))
            {
                Coord.Name = "JValue";
                Coord.value = CoordList[i].mid(1).toFloat();
                Coords.append(Coord);
            }
        }
    }
    return Coords;
}

void GCodeInterpreter::ParseLine(QString &GString)
{
    if(_TerminateThread)
        return;
    GString = GString.toUpper();
    QStringList GVals = GString.split(" ");

    if(GString.contains("F"))
    {
        if(_TerminateThread)
            return;
        for(int i = 0; i < GVals.length(); i++)
            if(GVals[i].contains("F") && !GVals[i].mid(1).isEmpty())
                //Convert to units per millisecond
                //F1500(mm/Min) = 25mm /Sec (1500 /60)
                //1500 / 60000 = .025
                _SpeedFactor =  GVals[i].mid(1).toFloat() / 60000;
        //TODO: Setup for speed multiplier from user input from UI.
    }

    if(GString.startsWith("G"))
    {
        if(_TerminateThread)
            return;
        if((GVals[0].mid(1) == "1" || GVals[0].mid(1) == "0"  || GVals[0].mid(1) == "01") && !GVals[0].mid(1).isEmpty())
        {
            emit ProcessingMoves("Printing...");
            QList<Coordinate> Coords = GetCoordValues(GString);
            float XVal =0, YVal =0, ZVal =0, ExtVal =0;
            for(int i =0; i < Coords.length(); i++)
            {
                if(Coords[i].Name == "XAxis")
                    XVal = Coords[i].value;
                if(Coords[i].Name == "YAxis")
                    YVal = Coords[i].value;
                if(Coords[i].Name == "ZAxis")
                    ZVal = Coords[i].value;
                if(Coords[i].Name == "ExtAxis")
                    ExtVal = Coords[i].value;
            }
            MoveToolHead(XVal, YVal, ZVal, ExtVal);
            emit MoveComplete("Move Complete");
        }

        else if (GVals[0].mid(1) == "2" || GVals[0].mid(1) == "3" || GVals[0].mid(1) == "02" || GVals[0].mid(1) == "03")
        {
            emit ProcessingMoves("Printing...");
            QList<Coordinate> Coords = GetCoordValues(GString);
            float XVal =0, YVal =0, ZVal =0, ExtVal =0, IValue = 0, JValue = 0;
            for(int i =0; i < Coords.length(); i++)
            {
                if(Coords[i].Name == "XAxis")
                    XVal = Coords[i].value;
                if(Coords[i].Name == "YAxis")
                    YVal = Coords[i].value;
                if(Coords[i].Name == "ZAxis")
                    ZVal = Coords[i].value;
                if(Coords[i].Name == "ExtAxis")
                    ExtVal = Coords[i].value;
                if(Coords[i].Name == "IValue")
                    IValue = Coords[i].value;
                if(Coords[i].Name == "JValue")
                    JValue = Coords[i].value;
            }
            if (GVals[0].mid(1) == "2" || GVals[0].mid(1) == "02")
                ExecuteArcMove(XVal,YVal,ZVal,ExtVal, IValue, JValue, GCodeInterpreter::CLOCKWISE);
            else
                ExecuteArcMove(XVal,YVal,ZVal,ExtVal, IValue, JValue, GCodeInterpreter::CTRCLOCKWISE);
        }

        else if (GVals[0].mid(1) == "4" || GVals[0].mid(1) == "04")
        {
            if(GVals[1].contains("S") && !GVals[1].mid(1).isEmpty())
                msleep((GVals[1].mid(1).toInt() * 1000));//Sleep for seconds
            else if(GVals[1].contains("P") && !GVals[1].mid(1).isEmpty())
                msleep(GVals[1].mid(1).toInt());//Sleep for miliseconds
        }

        else if(GVals[0].mid(1) == "10")
        {
            /*
                     Usage
                        G10 Pnnn Xnnn Ynnn Znnn Rnnn Snnn1
                    Parameters
                        Pnnn Tool number
                        Xnnn X offset
                        Ynnn Y offset
                        Znnn Z offset2
                        Rnnn Standby temperature(s)
                        Snnn Active temperature(s)
                    Examples
                        G10 P2 X17.8 Y-19.3 Z0.0 (sets the offset for tool (or in older implementations extrude head) 2 to the X, Y, and Z values specified)
                        G10 P1 R140 S205 (set standby and active temperatures3 for tool 1)

                    Remember that any parameter that you don't specify will automatically be set to the last value for that parameter. That usually means that you want explicitly to set Z0.0. RepRapFirmware will report the tool parameters if only the tool number is specified.

                    The R value is the standby temperature in oC that will be used for the tool, and the S value is its operating temperature. If you don't want the tool to be at a different temperature when not in use, set both values the same. See the T code (select tool) below. In tools with multiple heaters the temperatures for them all are specified thus: R100.0:90.0:20.0 S185.0:200.0:150.0 .

                    See also M585.

                    Notes

                    1Marlin uses G10/G11 for executing a retraction/unretraction move. The RepRapPro version of Marlin supports G10 for tool offset. Smoothie uses G10 for retract and G10 Ln for setting workspace coordinates.

                    2It's usually a bad idea to put a non-zero Z value in as well unless the tools are loaded and unloaded by some sort of tool changer. When all the tools are in the machine at once they should all be set to the same Z height.

                    3If the absolute zero temperature (-273.15) is passed as active and standby temperatures, RepRapFirmware will only switch off the tool heater(s) without changing their preset active or standby temperatures. RepRapFirmware-dc42 does not support this setting.
                 */
        }

        else if(GVals[0].mid(1) == "20")
        {
            emit ProcessingMoves("Working in Imperial Measure (SAE)");
            _XRes /= 25.4;
            _YRes /= 25.4;
            _ZRes /= 25.4;
            _ExtRes /= 25.4;
        }

        else if(GVals[0].mid(1) == "21")
            emit ProcessingMoves("Working in Metric Measure");

        else if(GVals[0].mid(1) == "28")
        {
            emit ProcessingMoves("Homing All Axis");
            HomeAllAxis();
        }

        else if(GVals[0].mid(1) == "90") //Set to absolute positioning
        {
            //TODO:
            emit ProcessingMoves("Using Absolute Positioning");
            emit PrintStarted();
        }

        else if(GVals[0].mid(1) == "91") //Set to relative positioning
        {
            //TODO:
            emit ProcessingMoves("Using Relative Positioning");
            emit PrintStarted();
        }

        else if(GVals[0].mid(1) == "92")
        {
            if(GVals.length() > 1)
            {
                if(GVals[1].startsWith("X") && !GVals[1].mid(1).isEmpty())
                {
                    _XAxis->Position = GVals[1].mid(1).toLong();
                    emit ProcessingMoves("Resetting X Axis Position to " + GVals[1].mid(1));
                }
                if(GVals[1].startsWith("Y") && !GVals[1].mid(1).isEmpty())
                {
                    _YAxis->Position = GVals[1].mid(1).toLong();
                    emit ProcessingMoves("Resetting Y Axis Position" + GVals[1].mid(1));
                }
                if(GVals[1].startsWith("Z") && !GVals[1].mid(1).isEmpty())
                {
                    _ZAxis->Position = GVals[1].mid(1).toLong();
                    emit ProcessingMoves("Resetting Z Axis Position" + GVals[1].mid(1));
                }
                if(GVals[1].startsWith("E") && !GVals[1].mid(1).isEmpty())
                {
                    _ExtAxis->Position = GVals[1].mid(1).toLong();
                    emit ProcessingMoves("Resetting Extruder Position" + GVals[1].mid(1));
                }
            }
            else
            {
                _XAxis->Position = 0;
                _YAxis->Position = 0;
                _ZAxis->Position = 0;
                _ExtAxis->Position = 0;
                emit ProcessingMoves("Resetting All Axis Positions to 0");
            }
        }

        else if(GVals[0].mid(1) == "161")
        {
            /*
                Parameters
                X Flag to home the X axis to its minimum position
                Y Flag to home the Y axis to its minimum position
                Z Flag to home the Z axis to its minimum position
                Fnnn Desired feedrate for this command
            Example
                G161 X Y Z F1800

            Instruct the machine to home the specified axes to their minimum position. Similar to G28, which decides on its own in which direction to search endstops.
             */
        }

        else if(GVals[0].mid(1) == "162")
        {
            /*
             Parameters
                X Flag to home the X axis to its maximum position
                Y Flag to home the Y axis to its maximum position
                Z Flag to home the Z axis to its maximum position
                Fnnn Desired feedrate for this command
            Example
                G162 X Y Z F1800

            Instruct the machine to home the specified axes to their maximum position.
             */
        }
    }

    else if(GString.startsWith("M"))
    {
        if(_TerminateThread)
            return;
        if(GVals[0].mid(1) == "0" || GVals[0].mid(1) == "1" || GVals[0].mid(1) == "01")
        {
            /*Parameters
                This command can be used without any additional parameters.
                Pnnn Time to wait, in milliseconds1
                Snnn Time to wait, in seconds2
                Example
                    M0
                */
            //TODO: Look for the wait time and respond to keys...
            TerminatePrint();
        }

        else if(GVals[0].mid(1) == "02" || GVals[0].mid(1) == "2" )
        {
            /*
                Finished Shutting off...
                 */
        }

        else if(GVals[0].mid(1) == "03" || GVals[0].mid(1) == "3" )
        {
            /*
                 *  Spindle On, Clockwise (CNC specific)
                     Parameters
                        Snnn Spindle RPM
                    Example
                        M3 S4000

                    The spindle is turned on with a speed of 4000 RPM.

                    Teacup firmware turn extruder on (same as M101).

                    RepRapFirmware interprets this code only if a Roland mill has been configured.

                    In Repetier-Firmware in laser mode you need S0..255 to set laser intensity. Normally you use S255 to turn it on full power for moves.
                    Laser will only fire during G1/2/3 moves and in laser mode (M452).
                 */
        }

        else if(GVals[0].mid(1) == "04" || GVals[0].mid(1) == "4" )
        {
            /*
                 * Spindle On, Counter-Clockwise (CNC specific)
                     Parameters
                        Snnn Spindle RPM
                    Example
                        M3 S4000

                    The spindle is turned on with a speed of 4000 RPM.

                    Teacup firmware turn extruder on (same as M101).

                    RepRapFirmware interprets this code only if a Roland mill has been configured.

                    In Repetier-Firmware in laser mode you need S0..255 to set laser intensity. Normally you use S255 to turn it on full power for moves.
                    Laser will only fire during G1/2/3 moves and in laser mode (M452).
                 */
        }

        else if(GVals[0].mid(1) == "05" || GVals[0].mid(1) == "5" )
        {
            /*
                Spindle Off (CNC specific)
                    Example: M5
                    The spindle is turned off.
                    Teacup firmware turn extruder off (same as M103).
                 */
        }
        else if(GVals[0].mid(1) == "06" || GVals[0].mid(1) == "6" )
        {
            /*
           * M6: Tool change
          */
        }
        else if(GVals[0].mid(1) == "07" || GVals[0].mid(1) == "7" )
        {
            /*
                Example: M7
                Mist coolant is turned on (if available)
                Teacup firmware turn on the fan, and set fan speed (same as M106).
              */
        }
        else if(GVals[0].mid(1) == "08" || GVals[0].mid(1) == "8" )
        {
            /*
        M8: Flood Coolant On (CNC specific)
        Example: M8
        Flood coolant is turned on (if available)
        */
        }
        else if(GVals[0].mid(1) == "09" || GVals[0].mid(1) == "9" )
        {
            /*
            M9: Coolant Off (CNC specific)
            Example: M9
            All coolant systems are turned off.
            */
        }
        else if(GVals[0].mid(1) == "10")
        {
            /*
            M10: Vacuum On (CNC specific)
            Example: M10
            Dust collection vacuum system turned on.
            */
        }
        else if(GVals[0].mid(1) == "11")
        {
            /*
            M11: Vacuum Off (CNC specific)
            Example: M11
            Dust collection vacuum system turned off.
            */
        }
        else if(GVals[0].mid(1) == "37" )
        {
            /*
            M37: Simulation mode
            Used to switch between printing mode and simulation mode. Simulation mode allows the electronics to compute an accurate printing time,
            taking into account the maximum speeds, accelerations etc. that are configured.
            M37 S1 enters simulation mode. All G and M codes will not be acted on, but the time they take to execute will be calculated.

            M37 S0 leaves simulation mode.

            M37 with no S parameter prints the time taken by the simulation, from the time it was first entered using M37 S1,
            up to the current point (if simulation mode is still active) or the point that the simulation was ended (if simulation mode is no longer active).
            */
        }
        else if(GVals[0].mid(1) == "80" )
        {
            /*
             M80: ATX Power On
            Example
                M80
            Turns on the ATX power supply from standby mode to fully operational mode. No-op on electronics without standby mode.
            */
        }
        else if(GVals[0].mid(1) == "81" )
        {
            /*
            M81: ATX Power Off
            Example
                M81
            Turns off the ATX power supply. Counterpart to M80.
            */
        }
        else if(GVals[0].mid(1) == "84" )
        {
            /*
            M84: Stop idle hold
            Parameters
               This command can be used without any additional parameters.
                Innn Reset flags1
            Example
                M84
            Stop the idle hold on all axis and extruder. In some cases the idle hold causes annoying noises, which can be stopped by disabling the hold.
            Be aware that by disabling idle hold during printing, you will get quality issues. This is recommended only in between or after printjobs.
            */
        }
        else if(GVals[0].mid(1) == "104")
        {
            _ExtruderTemp = GVals[1].mid(1).toInt();
            SetExtruderTemp(GVals[1].mid(1).toInt());
            emit ExtruderTemperatureChanged(this->ExtProbeWorker->GetTargetTemp());
        }

        else if(GVals[0].mid(1) == "106")
        {
            //Fan on
        }

        else if(GVals[0].mid(1) == "107")
        {
            //Fan Off
        }

        else if(GVals[0].mid(1) == "109")
        {
            if(!ExtProbeWorker->isRunning())
                ExtProbeWorker->start();
            _ExtruderTemp = GVals[1].mid(1).toInt();
            SetExtruderTemp(GVals[1].mid(1).toInt());
            emit ExtruderTemperatureChanged(this->ExtProbeWorker->GetTargetTemp());
            //We need to wait until the temp is reached before proceeding...
            emit ProcessingTemps("Spinning up Heaters");
            if(ExtProbeWorker->GetCurrentTemp() > ExtProbeWorker->GetTargetTemp())
                while(ExtProbeWorker->GetCurrentTemp() > ExtProbeWorker->GetTargetTemp())
                {
                    if(_TerminateThread)
                        return;
                    emit TemperatureHigh(ExtProbeWorker->GetCurrentTemp() - ExtProbeWorker->GetTargetTemp());
                    msleep(100);//sleep for 100 MS while we wait
                }
            else
                while(ExtProbeWorker->GetCurrentTemp() < ExtProbeWorker->GetTargetTemp())
                {
                    if(_TerminateThread)
                        return;
                    emit TemperatureLow(ExtProbeWorker->GetCurrentTemp() - ExtProbeWorker->GetTargetTemp());
                    msleep(100);//sleep for 100 MS while we wait
                }
        }

        else if(GVals[0].mid(1) == "140")
        {
            _BedTemp = GVals[1].mid(1).toInt();
            SetBedTemp(GVals[1].mid(1).toInt());
            emit BedTemperatureChanged( this->BedProbeWorker->GetTargetTemp());

        }

        else if(GVals[0].mid(1) == "190")
        {
            if(!BedProbeWorker->isRunning())
                BedProbeWorker->start();
            _BedTemp = GVals[1].mid(1).toInt();
            SetBedTemp(GVals[1].mid(1).toInt());
            emit BedTemperatureChanged( this->BedProbeWorker->GetTargetTemp());
            //We need to wait until the temp is reached before proceeding...
            emit ProcessingTemps("Spinning up Heaters");
            if(BedProbeWorker->GetCurrentTemp() > BedProbeWorker->GetTargetTemp())
                while(BedProbeWorker->GetCurrentTemp() > BedProbeWorker->GetTargetTemp())
                {
                    if(_TerminateThread)
                        return;
                    emit TemperatureHigh(BedProbeWorker->GetCurrentTemp() - BedProbeWorker->GetTargetTemp());
                    msleep(100);//sleep for 100 MS while we wait
                }
            else
                while(BedProbeWorker->GetCurrentTemp() < BedProbeWorker->GetTargetTemp())
                {
                    if(_TerminateThread)
                        return;
                    emit TemperatureLow(BedProbeWorker->GetCurrentTemp() - BedProbeWorker->GetTargetTemp());
                    msleep(100);//sleep for 100 MS while we wait
                }
        }
        else if(GVals[0].mid(1) == "126")
        {
            /*
            M126: Open Valve
            Example: M126 P500
            Open the extruder's valve (if it has one) and wait 500 milliseconds for it to do so.

            M126 in MakerBot
            Example: M126 T0
            Enables an extra output attached to a specific toolhead (e.g. fan)
            */
        }
        else if(GVals[0].mid(1) == "127")
        {
            /*
            M127: Close Valve
            Example: M127 P400
            Close the extruder's valve (if it has one) and wait 400 milliseconds for it to do so.
            Example: M127 T0
            Disables an extra output attached to a specific toolhead (e.g. fan)
            */
        }
        else if(GVals[0].mid(1) == "141")
        {
            /*
             M141: Set Chamber Temperature (Fast)
            Parameters
                Snnn Target temperature
                Hnnn Heater number1
            Examples
                M141 S30
                M141 H0
            Set the temperature of the chamber to 30oC and return control to the host immediately (i.e. before that temperature has been reached by the chamber).
            Notes
            1This parameter is only available in RepRapFirmware.
            */
        }
        else if(GVals[0].mid(1) == "191")
        {
            /*
            M191: Wait for chamber temperature to reach target temp
            Example: M191 P60
            Set the temperature of the build chamber to 60 °C and wait for the temperature to be reached.
            */
        }
        else if(GVals[0].mid(1) == "240")
        {
            /*
             M240: Trigger camera
            Example: M240
            Triggers a camera to take a photograph. (Add to your per-layer GCode.)
            */
        }
        else if(GVals[0].mid(1) == "300")
        {
            /*
             M300: Play beep sound
            Parameters
                Snnn frequency in Hz
                Pnnn duration in milliseconds
            Example
                M300 S300 P1000
            Play beep sound, use to notify important events like the end of printing. See working example on R2C2 electronics.
            If an LCD device is attached to RepRapFirmware, a sound is played via the add-on touch screen control panel. Else the web interface will play a beep sound.
            */
        }
        else if(GVals[0].mid(1) == "380")
        {
            /*
             M380: Activate solenoid
            Example: M380
            Activates solenoid on active extruder.
            */
        }
        else if(GVals[0].mid(1) == "452")
        {
            /*
                 M452: Select Laser Printer Mode
                Usage
                    M452
                Example
                    M452
                Output
                    PrinterMode:Laser
                Switches to laser mode. This mode enables handling of a laser pin and makes sure that the laser is only activated during G1 moves
                if laser was enabled or E is increasing. G0 moves should never enable the laser. M3/M5 can be used to enable/disable the laser for moves.
                */
        }
        else if(GVals[0].mid(1) == "453")
        {
            /*
                 M453: Select CNC Printer Mode
                Usage_Clock
                    M453
                Example
                    M453
                Output
                    PrinterMode:CNC
                Switches to CNC mode. In this mode M3/M4/M5 control the pins defined for the milling device.
                */
        }
    }
    emit EndLineProcessing("Line Processed");
}

void GCodeInterpreter::ExecutePrintSequence()
{
    long long int LineCounter = 0;
    int Progress = 0;

    if(_XAxis->IsAgainstStop() || _YAxis->IsAgainstStop() || _ZAxis->IsAgainstStop() || _ExtAxis->IsAgainstStop())
    {
        QMessageBox::critical(0, "Ambigious End Stop Detected!", "An end stop on one of the axis is triggered and PiRinter3D cannot determine which one it is. Please check the machine configuration and manually move the carriages from the end stops to continue!", QMessageBox::Ok);
        _TerminateThread = true;
    }

    while(LineCounter < _GCODE.length() && !_TerminateThread)
    {
        if(_TerminateThread)
            return;
        while(_Stop)
        {
            if(_TerminateThread)
                return;
            emit ProcessingMoves("Paused");
            msleep(50);
        }
        //        if(ProcessUserCommand)
        //        {
        //Process any user commands here then follow through with the Parse line.
        //        }
        emit BeginLineProcessing(_GCODE[LineCounter]);
        ParseLine(_GCODE[LineCounter]);
        LineCounter ++;
        Progress = (int)(LineCounter * (100.00 / _GCODE.length()));
        emit ReportProgress(Progress);
        emit ReportMotorPosition(QString::fromStdString(_XAxis->MotorName), _XAxis->Position );
        emit ReportMotorPosition(QString::fromStdString(_YAxis->MotorName), _YAxis->Position );
        emit ReportMotorPosition(QString::fromStdString(_ZAxis->MotorName), _ZAxis->Position );
        emit ReportMotorPosition(QString::fromStdString(_ExtAxis->MotorName), _ExtAxis->Position );
    }

    if(!_TerminateThread)
    {
        _SpeedFactor = .15;
        //Extend the bed so we can remove the part.
        //MoveToolHead(1, _YArea - 50, 0, _ExtAxis->Position - 5);
        emit ReportProgress(100);
    }
}
//======================================End Private Methods========================================

//===========================================SLOTS=================================================
void GCodeInterpreter::BeginPrint()
{
    if(!_IsPrinting)
    {
       // connect(_Controller, SIGNAL(ReportMotorPosition(QString,long)), this, SLOT(UpdatePositionLabel(QString,long)));
        _IsPrinting = true;
        ExecutePrintSequence();
        BedProbeWorker->Terminate();
        ExtProbeWorker->Terminate();
        BedProbeWorker->wait();
        ExtProbeWorker->wait();
        emit PrintComplete();
    }
}

void GCodeInterpreter::PausePrint()
{
    if(this->_Stop)
        this->_Stop = false;
    else
        this->_Stop = true;
}

void GCodeInterpreter::TerminatePrint()
{
    this->_TerminateThread = true;
    BedProbeWorker->Terminate();
    ExtProbeWorker->Terminate();
    BedProbeWorker->wait();
    ExtProbeWorker->wait();
}

void GCodeInterpreter::ChangeBedTemp(const int &Celsius)
{
    this->BedProbeWorker->SetTargetTemp(Celsius);
}

void GCodeInterpreter::ChangeExtTemp(const int &Celsius)
{
    this->ExtProbeWorker->SetTargetTemp(Celsius);
}

void GCodeInterpreter::UpdatePositionLabel(QString Name, const long Pos)
{
    emit ReportMotorPosition(Name, Pos);
}

//=======================================END SLOTS=================================================
