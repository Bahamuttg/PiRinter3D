#include "gcodeinterpreter.h"
#include "thermalprobe.h"
#include "steppermotor.h"
#include "motorcontroller.h"

GCodeInterpreter::GCodeInterpreter(const QString &FilePath, QObject *parent)
    :QThread(parent)
{
    _XAxis = 0;
    _YAxis = 0;
    _ZAxis = 0;
    _ExtAxis = 0;
    _ExtProbe = 0;
    _BedProbe = 0;
    BedProbeWorker = 0;
    ExtProbeWorker = 0;
    _XStop = 0;
    _YStop = 0;
    _ZStop = 0;

    _ExtRes = .20;
    _XRes = .20;
    _YRes = .20;
    _ZRes = .20;

    _IsPrinting = false;
    _Stop = false;
    _TerminateThread = false;

    InitializeMotors();
    InitializeEndStops();
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
    delete _XStop;
    delete _YStop;
    delete _ZStop;
    delete BedProbeWorker;
    delete ExtProbeWorker;
}

void GCodeInterpreter::LoadGCode(const QString &FilePath)
{
    QFile PrintFile(FilePath);
    if(PrintFile.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        QTextStream Stream(&PrintFile);
        while (!Stream.atEnd())
        {
            QString Line = Stream.readLine();
            if(!Line.startsWith(";") && !Line.isEmpty())//Weed out the comment lines and empties.
                _GCODE << Line.split(";")[0];//Again weed out any comments, we want just raw GCODE.

            /*
             * M190 S60 ; set bed temperature
             * M104 S205 ; set temperature
             */
            if(this->BedProbeWorker->GetTargetTemp() == 0 || this->ExtProbeWorker->GetTargetTemp() == 0)
                if(Line.split(" ")[0].toUpper().startsWith("M"))
                {
                    QStringList MCode = Line.split(" ");
                    if(MCode[0].toUpper() == "M190")
                    {
                        this->BedProbeWorker->SetTargetTemp(MCode[1].mid(1).toInt());
                        emit BedTemperatureChanged( this->BedProbeWorker->GetTargetTemp());
                    }
                    if(MCode[0].toUpper() == "M104")
                    {
                        this->ExtProbeWorker->SetTargetTemp(MCode[1].mid(1).toInt());
                        emit ExtruderTemperatureChanged(this->ExtProbeWorker->GetTargetTemp());
                    }
                }
        }
        PrintFile.close();
    }
}

void GCodeInterpreter::run()
{
    BeginPrint();
}

//======================================Private Methods============================================
    void GCodeInterpreter::InitializeEndStops()
    {
    }

    void GCodeInterpreter::InitializeThermalProbes()
    {
        QFile ProbeConfig("ProbeCfg.ini");
        if(ProbeConfig.open(QIODevice::ReadOnly | QIODevice::Text ))
        {
            QTextStream CfgStream(&ProbeConfig); //load config file
            while (!CfgStream.atEnd())
            {
                QString Line = CfgStream.readLine(); //read one line at a time
                if(Line.contains("ProbeConfig"))
                {
                    QStringList Params = Line.split(";");
                    if(Params[0].contains("ExtruderProbe"))
                    {
                       _ExtProbe = new ThermalProbe(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt());
                    }
                    if(Params[0].contains("BedProbe"))
                    {
                       _BedProbe = new ThermalProbe(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt());
                    }
                }
            }
            ProbeConfig.close();
        }
        else
        {
            //Debugging Code
            _BedProbe = new ThermalProbe(0, 100, 0, 0);
            _ExtProbe = new ThermalProbe(0, 101, 0, 0);
             BedProbeWorker = new ProbeWorker(_BedProbe, 100, this);
             ExtProbeWorker = new ProbeWorker(_ExtProbe, 100, this);
        }
        connect(this, SIGNAL(terminated()), BedProbeWorker, SLOT(terminate()));
        connect(this, SIGNAL(terminated()), ExtProbeWorker, SLOT(terminate()));
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
                            this->_XAxis = new StepperMotor(Params[1].toInt(),  Params[3].toInt(), Params[9].toInt(), Params[0].split("::")[1].toStdString());
                        else
                            this->_XAxis = new StepperMotor(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(),
                                    Params[9].toInt(), Params[0].split("::")[1].toStdString());
                    }
                    if(Params[0].contains("YAxis"))
                    {
                        if(Params[6].toInt())
                            this->_YAxis = new StepperMotor(Params[1].toInt(),  Params[3].toInt(), Params[9].toInt(), Params[0].split("::")[1].toStdString());
                        else
                            this->_YAxis = new StepperMotor(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(),
                                    Params[9].toInt(), Params[0].split("::")[1].toStdString());
                    }
                    if(Params[0].contains("ZAxis"))
                    {
                        if(Params[6].toInt())
                            this->_ZAxis = new StepperMotor(Params[1].toInt(),  Params[3].toInt(), Params[9].toInt(), Params[0].split("::")[1].toStdString());
                        else
                            this->_ZAxis = new StepperMotor(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(),
                                    Params[9].toInt(), Params[0].split("::")[1].toStdString());
                    }
                    if(Params[0].contains("ExtAxis"))
                    {
                        if(Params[6].toInt())
                            this->_ExtAxis = new StepperMotor(Params[1].toInt(),  Params[3].toInt(), Params[9].toInt(), Params[0].split("::")[1].toStdString());
                        else
                            this->_ExtAxis = new StepperMotor(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(),
                                    Params[9].toInt(), Params[0].split("::")[1].toStdString());
                    }
                }
            }
            MotorConfig.close();
        }
    }

    void GCodeInterpreter::HomeAllAxis()
    {
    }

    void GCodeInterpreter::ChangeTemps(const int &ExtruderCelsius, const int &BedCelsius)
    {
        this->BedProbeWorker->SetTargetTemp(BedCelsius);
        this->ExtProbeWorker->SetTargetTemp(ExtruderCelsius);
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
        float total_3dsteps = 0;

        if(total_steps != 0 && total_3dsteps != 0 && stepext != 0)
            //Future Overload the Step motors to contain a delay for each individual motor based upon their individual Resolution.
             _Controller.StepMotors(*_XAxis, stepx, *_YAxis, stepy, *_ZAxis, stepz, *_ExtAxis, stepext, _SpeedFactor / qMin(_XRes, _YRes));
        else if(total_steps != 0 && stepext != 0)
            _Controller.StepMotors(*_XAxis, stepx, *_YAxis, stepy, *_ExtAxis, stepext, _SpeedFactor / qMin(_XRes, _YRes));
        else if(total_steps != 0)
            _Controller.StepMotors(*_XAxis, stepx, *_YAxis, stepy, _SpeedFactor / qMin(_XRes, _YRes));
        else
        {
            _Controller.StepMotor(*_ZAxis, stepz, _SpeedFactor / _ZRes);
            _Controller.StepMotor(*_ExtAxis, stepext, _SpeedFactor / _ExtRes);
        }
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
            for(int i = 0; i < GVals.length(); i++)
                if(GVals[i].contains("F") && !GVals[i].mid(1).isEmpty())
                    //convert to units per second
                    _SpeedFactor =  GVals[i].mid(1).toFloat() / 60;
        }

        if(GString.startsWith("G"))
        {
            if(GVals[0].mid(1) == "1" && !GVals[0].mid(1).isEmpty())
            {
                emit ProcessingMoves("Moving Head --- " + GString);
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
                emit MoveComplete("Move Complete --- " + GString);
            }

            else if (GVals[0].mid(1) == "2" || GVals[0].mid(1) == "3")
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
            sintheta=(Dx*e2[0]+Dy*e2[1])/r**2;        #theta is the angule spanned by the circular interpolation curve

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

            else if(GVals[0].mid(1) == "90")
                emit PrintStarted();

            else if(GVals[0].mid(1) == "92")
            {
                emit ProcessingMoves("Resetting Extruder Position");
                if(GVals.length() > 2)
                {
                    if(GVals[1].mid(1) == "E" && !GVals[1].mid(1).isEmpty())
                        _ExtAxis->Position = GVals[1].mid(1).toLong();
                }
                else
                    _ExtAxis->Position = 0;
            }
        }

        else if(GString.startsWith("M"))
        {
            if(GVals[0].mid(1) == "02")
            {
                /*
                Finished Shutting off...
                 */
            }

            else if(GVals[0].mid(1) == "03")
            {
                /*
                ToolOn(MZ)
                #GPIO.output(Laser_switch,False);
                print 'Pen turned off';
                 */
            }

            else if(GVals[0].mid(1) == "05")
            {
                /*
                ToolOff(MZ)
                #GPIO.output(Laser_switch,False);
                print 'Pen turned off';
                 */
            }

            else if(GVals[0].mid(1) == "104")
            {
                this->ExtProbeWorker->SetTargetTemp(GVals[1].mid(1).toInt());
                emit SetExtruderTemp(GVals[1].mid(1).toInt());
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
                this->ExtProbeWorker->SetTargetTemp(GVals[1].mid(1).toInt());
                emit SetExtruderTemp(GVals[1].mid(1).toInt());
                emit ExtruderTemperatureChanged(this->ExtProbeWorker->GetTargetTemp());
                //We need to wait until the temp is reached before proceeding...
                if(ExtProbeWorker->GetCurrentTemp() > ExtProbeWorker->GetTargetTemp())
                    while(ExtProbeWorker->GetCurrentTemp() > ExtProbeWorker->GetTargetTemp())
                    {
                        emit TemperatureHigh(ExtProbeWorker->GetCurrentTemp() - ExtProbeWorker->GetTargetTemp());
                        msleep(100);//sleep for 100 MS while we wait
                    }
                else
                    while(ExtProbeWorker->GetCurrentTemp() < ExtProbeWorker->GetTargetTemp())
                    {
                        emit TemperatureLow(ExtProbeWorker->GetCurrentTemp() - ExtProbeWorker->GetTargetTemp());
                        msleep(100);//sleep for 100 MS while we wait
                    }
            }

            else if(GVals[0].mid(1) == "140")
            {
                this->BedProbeWorker->SetTargetTemp(GVals[1].mid(1).toInt());
                emit SetBedTemp(GVals[1].mid(1).toInt());
                emit BedTemperatureChanged( this->BedProbeWorker->GetTargetTemp());

            }

            else if(GVals[0].mid(1) == "190")
            {
                this->BedProbeWorker->SetTargetTemp(GVals[1].mid(1).toInt());
                emit SetBedTemp(GVals[1].mid(1).toInt());
                emit BedTemperatureChanged( this->BedProbeWorker->GetTargetTemp());
                //We need to wait until the temp is reached before proceeding...
                if(BedProbeWorker->GetCurrentTemp() > BedProbeWorker->GetTargetTemp())
                    while(BedProbeWorker->GetCurrentTemp() > BedProbeWorker->GetTargetTemp())
                    {
                        emit TemperatureHigh(BedProbeWorker->GetCurrentTemp() - BedProbeWorker->GetTargetTemp());
                        msleep(100);//sleep for 100 MS while we wait
                    }
                else
                    while(BedProbeWorker->GetCurrentTemp() < BedProbeWorker->GetTargetTemp())
                    {
                        emit TemperatureLow(BedProbeWorker->GetCurrentTemp() - BedProbeWorker->GetTargetTemp());
                        msleep(100);//sleep for 100 MS while we wait
                    }
            }
        }
    }

	void GCodeInterpreter::ExecutePrintSequence()
	{
        //Move some of these outside of the loop for threading and pausing purposes.
        int LineCounter = 0, Progress = 0;
        while(LineCounter < _GCODE.length() && !_TerminateThread)
        {
            if(_TerminateThread)
                return;
            while(_Stop)
            {
                emit MoveComplete("Paused");
                //msleep(50);
            }
            emit BeginLineProcessing(_GCODE[LineCounter]);
            ParseLine(_GCODE[LineCounter]);
            //Debugging
            //msleep(50);
            LineCounter ++;
            Progress = (int)(LineCounter * (100.00 / _GCODE.length()));
            emit ReportProgress(Progress);
        }
        emit ReportProgress(100);
	}
//======================================End Private Methods========================================

//===========================================SLOTS=================================================

    void GCodeInterpreter::BeginPrint()
    {
        if(!_IsPrinting)
        {
            _IsPrinting = true;
            //Start the temp pull-up  by spinning up the ThermalProbe threads.
            emit ProcessingTemps("Spinning up Heaters");
            emit BedTemperatureChanged( this->BedProbeWorker->GetTargetTemp());
            emit ExtruderTemperatureChanged(this->ExtProbeWorker->GetTargetTemp());
            //Fire the threads
            BedProbeWorker->start();
            ExtProbeWorker->start();
//            while(ExtProbeWorker->GetCurrentTemp() < ExtProbeWorker->GetTargetTemp()
//                  && BedProbeWorker->GetCurrentTemp() < BedProbeWorker->GetTargetTemp())
//                msleep(500);
            emit ProcessingMoves("Homing All Axis");
            //Next it should home the motors.
            HomeAllAxis();
            //possibly validate the functionality of the end stops too.

            ExecutePrintSequence();
            BedProbeWorker->Terminate();
            ExtProbeWorker->Terminate();
            BedProbeWorker->wait();
            ExtProbeWorker->wait();
            emit PrintComplete();
            TerminatePrint();
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
        BedProbeWorker->terminate();
        ExtProbeWorker->terminate();
        BedProbeWorker->wait();
        ExtProbeWorker->wait();
    }
//=======================================END SLOTS=================================================
