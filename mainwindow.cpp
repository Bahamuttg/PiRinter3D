#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stepperdriver.h"
#include "motorconfigdialog.h"
#include "gcodeinterpreter.h"
#include "probeconfigdialog.h"
#include "printareaconfigdialog.h"
#include "heaterdriver.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    _ProgressBar = new QProgressBar(this);
    _ProgressBar->setRange(0,100);
    _ProgressBar->reset();
    _StatusLabel = new QLabel(this);
    _StatusLabel->setText("Ready");
    ui->setupUi(this);

    ui->lcdBedTemp->display("Off");
    ui->lcdExtruderTemp->display("Off");
    ui->statusBar->insertPermanentWidget(0, _ProgressBar, 100);
    ui->statusBar->insertPermanentWidget(1, _StatusLabel, 250);
    ui->lblBedOn->setVisible(false);
    ui->lblExtOn->setVisible(false);

    _XButtonTimer = new QTimer(this);
    _XButtonTimer->setInterval(300);
    _YButtonTimer = new QTimer(this);
    _YButtonTimer->setInterval(300);
    _ZButtonTimer = new QTimer(this);
    _ZButtonTimer->setInterval(300);
    _ExtButtonTimer = new QTimer(this);
    _ExtButtonTimer->setInterval(300);

    _XWorker = 0;
    _YWorker = 0;
    _ZWorker = 0;
    _ExtWorker = 0;

    _XMotorThread = 0;
    _YMotorThread = 0;
    _ZMotorThread = 0;
    _ExtMotorThread = 0;

    _Interpreter = 0;

    //Link the Timer elapsed to the Timer stop so we can use it again.
    connect(_XButtonTimer, SIGNAL(timeout()), _XButtonTimer, SLOT(stop()));
    connect(_YButtonTimer, SIGNAL(timeout()), _YButtonTimer, SLOT(stop()));
    connect(_ZButtonTimer, SIGNAL(timeout()), _ZButtonTimer, SLOT(stop()));
    connect(_ExtButtonTimer, SIGNAL(timeout()), _ExtButtonTimer, SLOT(stop()));

    //Link the Buttons pressed signal to the timer start
    connect(ui->btnXLeft, SIGNAL(pressed()), _XButtonTimer, SLOT(start()));
    connect(ui->btnXRight, SIGNAL(pressed()), _XButtonTimer, SLOT(start()));
    connect(ui->btnYBack, SIGNAL(pressed()), _YButtonTimer, SLOT(start()));
    connect(ui->btnYFore, SIGNAL(pressed()), _YButtonTimer, SLOT(start()));
    connect(ui->btnZDown, SIGNAL(pressed()), _ZButtonTimer, SLOT(start()));
    connect(ui->btnZUp, SIGNAL(pressed()), _ZButtonTimer, SLOT(start()));
    connect(ui->btnEXTBack, SIGNAL(pressed()), _ExtButtonTimer, SLOT(start()));
    connect(ui->btnEXTFore, SIGNAL(pressed()), _ExtButtonTimer, SLOT(start()));

    //Link the Buttons released signal to the ButtonTimer stop.
    connect(ui->btnXLeft, SIGNAL(released()), _XButtonTimer, SLOT(stop()));
    connect(ui->btnXRight, SIGNAL(released()), _XButtonTimer, SLOT(stop()));
    connect(ui->btnYBack, SIGNAL(released()), _YButtonTimer, SLOT(stop()));
    connect(ui->btnYFore, SIGNAL(released()), _YButtonTimer, SLOT(stop()));
    connect(ui->btnZDown, SIGNAL(released()), _ZButtonTimer, SLOT(stop()));
    connect(ui->btnZUp, SIGNAL(released()), _ZButtonTimer, SLOT(stop()));
    connect(ui->btnEXTBack, SIGNAL(released()), _ExtButtonTimer, SLOT(stop()));
    connect(ui->btnEXTFore, SIGNAL(released()), _ExtButtonTimer, SLOT(stop()));

    InitializeMotors();
    InitializeMotorWorkers();
}

MainWindow::~MainWindow()
{
    if(_Interpreter != 0)
    {
        _Interpreter->TerminatePrint();
        _Interpreter->wait();
    }
    delete _XWorker;
    delete _YWorker;
    delete _ZWorker;
    delete _ExtWorker;

    delete _XAxis;
    delete _YAxis;
    delete _ZAxis;
    delete _ExtAxis;

    delete _XMotorThread;
    delete _YMotorThread;
    delete _ZMotorThread;
    delete _ExtMotorThread;

    delete _XButtonTimer;
    delete _YButtonTimer;
    delete _ZButtonTimer;
    delete _ExtButtonTimer;

    delete ui;
}
//==========================Private Methods==========================================
void MainWindow::LoadConfigurations()
{
    //TODO Load Configs.
}
void MainWindow::SaveConfigurations()
{
    //TODO Save Configs.
}
void MainWindow::InitializeMotors()
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
                    this->_ExtAxis->SetHoldOnIdle(Params[17].toInt());
                }
            }
        }
        MotorConfig.close();
    }
    else
        QMessageBox::critical(0, "Error Opening Motor Config File!", MotorConfig.errorString(), QMessageBox::Ok);
}
void MainWindow::InitializeMotorWorkers()
{
    if (_XWorker != 0)
    {
        if(_XWorker->IsBusy)
            _XWorker->Stop();
        delete _XWorker;
    }
    if (_YWorker != 0)
    {
        if(_YWorker->IsBusy)
            _YWorker->Stop();
        delete _YWorker;
    }
    if (_ZWorker != 0)
    {
        if(_ZWorker->IsBusy)
            _ZWorker->Stop();
        delete _ZWorker;
    }
    if (_ExtWorker != 0)
    {
        if(_ExtWorker->IsBusy)
            _ExtWorker->Stop();
        delete _ExtWorker;
    }

    if(_XMotorThread != 0)
        delete _XMotorThread;
    if(_YMotorThread != 0)
        delete _YMotorThread;
    if(_ZMotorThread != 0)
        delete _ZMotorThread;
    if(_ExtMotorThread != 0)
        delete _ExtMotorThread;


    _XMotorThread = new QThread(this);
    _YMotorThread = new QThread(this);
    _ZMotorThread = new QThread(this);
    _ExtMotorThread = new QThread(this);

    _XWorker = new MotorWorker(_XAxis);
    _XWorker->moveToThread(_XMotorThread);
    _YWorker = new MotorWorker(_YAxis);
    _YWorker->moveToThread(_YMotorThread);
    _ZWorker = new MotorWorker(_ZAxis);
    _ZWorker->moveToThread(_ZMotorThread);
    _ExtWorker = new MotorWorker(_ExtAxis);
    _ExtWorker->moveToThread(_ExtMotorThread);

    //Link the MotorThreads to their workers DoWork Slot.
    connect(_XMotorThread, SIGNAL(started()), _XWorker, SLOT(DoWork()));
    connect(_XMotorThread, SIGNAL(terminated()), _XWorker, SLOT(Stop()));
    connect(_XWorker, SIGNAL(ProgressChanged(QString,long)), this, SLOT(UpdatePositionLabel(QString,long)));
    connect(_YMotorThread, SIGNAL(started()), _YWorker, SLOT(DoWork()));
    connect(_YMotorThread, SIGNAL(terminated()), _YWorker, SLOT(Stop()));
    connect(_YWorker, SIGNAL(ProgressChanged(QString,long)), this, SLOT(UpdatePositionLabel(QString,long)));
    connect(_ZMotorThread, SIGNAL(started()), _ZWorker, SLOT(DoWork()));
    connect(_ZMotorThread, SIGNAL(terminated()), _ZWorker, SLOT(Stop()));
    connect(_ZWorker, SIGNAL(ProgressChanged(QString,long)), this, SLOT(UpdatePositionLabel(QString,long)));
    connect(_ExtMotorThread, SIGNAL(started()), _ExtWorker, SLOT(DoWork()));
    connect(_ExtMotorThread, SIGNAL(terminated()), _ExtWorker, SLOT(Stop()));
    connect(_ExtWorker, SIGNAL(ProgressChanged(QString,long)), this, SLOT(UpdatePositionLabel(QString, long)));

    //Link the Timer elapsed to the MotorThreads start.
    connect(_XButtonTimer, SIGNAL(timeout()), _XMotorThread, SLOT(start()));
    connect(_YButtonTimer, SIGNAL(timeout()), _YMotorThread, SLOT(start()));
    connect(_ZButtonTimer, SIGNAL(timeout()), _ZMotorThread, SLOT(start()));
    connect(_ExtButtonTimer, SIGNAL(timeout()), _ExtMotorThread, SLOT(start()));

    //Link the Buttons released signal to the Workers Stop Slot
    connect(ui->btnXLeft, SIGNAL(released()), _XWorker, SLOT(stop()));
    connect(ui->btnXRight, SIGNAL(released()), _XWorker, SLOT(stop()));
    connect(ui->btnYBack, SIGNAL(released()), _YWorker, SLOT(stop()));
    connect(ui->btnYFore, SIGNAL(released()), _YWorker, SLOT(stop()));
    connect(ui->btnZUp, SIGNAL(released()), _ZWorker, SLOT(stop()));
    connect(ui->btnZDown, SIGNAL(released()), _ZWorker, SLOT(stop()));
    connect(ui->btnEXTBack, SIGNAL(released()), _ExtWorker, SLOT(stop()));
    connect(ui->btnEXTFore, SIGNAL(released()), _ExtWorker, SLOT(stop()));

    //Link the Buttons released signal to the MotorThread quit.
    connect(ui->btnXLeft, SIGNAL(released()), _XMotorThread, SLOT(quit()));
    connect(ui->btnXRight, SIGNAL(released()), _XMotorThread, SLOT(quit()));
    connect(ui->btnYBack, SIGNAL(released()), _YMotorThread, SLOT(quit()));
    connect(ui->btnYFore, SIGNAL(released()), _YMotorThread, SLOT(quit()));
    connect(ui->btnZUp, SIGNAL(released()), _ZMotorThread, SLOT(quit()));
    connect(ui->btnZDown, SIGNAL(released()), _ZMotorThread, SLOT(quit()));
    connect(ui->btnEXTBack, SIGNAL(released()), _ExtMotorThread, SLOT(quit()));
    connect(ui->btnEXTFore, SIGNAL(released()), _ExtMotorThread, SLOT(quit()));
}
//==========================End Private Methods=======================================
//==============SLOTS===============================================================
//==========================Main Menu Handlers========================================
void MainWindow::on_action_Stepper_Utility_triggered()
{
    StepperDriver *SD = new StepperDriver(this);
    SD->show();
}
void MainWindow::on_actionPre_Heat_Elements_triggered()
{
        HeaterDriver *HD = new HeaterDriver(this);
        HD->show();
}
void MainWindow::on_action_Exit_triggered()
{
    if(_Interpreter->IsPrinting())
        _Interpreter->TerminatePrint();
    _Interpreter->terminate();
    this->close();
}
void MainWindow::on_action_Load_3D_Print_triggered()
{
    if(_Interpreter != 0)
        if(_Interpreter->IsPrinting())
        {
            QMessageBox Msg(this);
            Msg.setText("A Print is currently in process!\nStop the current print operation before loading a new print!");
            Msg.exec();
            return;
        }
    QFileDialog FD(this);
    FD.setFileMode(QFileDialog::ExistingFile);
    FD.setNameFilter(tr("G-Code files (*.gcode);;Text files (*.txt)"));
    FD.setViewMode(QFileDialog::Detail);
    FD.show();
    if(FD.exec())
    {
        _PrintFilePath = FD.selectedFiles()[0];
        if(_Interpreter != 0)
            delete _Interpreter;
        _StatusLabel->setText("Processing GCODE...");
        _Interpreter = new GCodeInterpreter(_PrintFilePath, this);
        ui->seBed->setValue(_Interpreter->BedProbeWorker->GetTargetTemp());
        ui->seExt->setValue(_Interpreter->ExtProbeWorker->GetTargetTemp());
        _StatusLabel->setText("Processed " + QString::number(_Interpreter->GetTotalLines()) + " Lines of GCODE...");
        connect(_Interpreter, SIGNAL(BedTemperatureChanged(int)), ui->seBed, SLOT(setValue(int)));
        connect(_Interpreter, SIGNAL(ExtruderTemperatureChanged(int)), ui->seExt, SLOT(setValue(int)));
        connect(ui->action_Stop, SIGNAL(triggered()), _Interpreter, SLOT(TerminatePrint()));
    }
}
void MainWindow::on_action_Configure_Motors_triggered()
{
    MotorConfigDialog Dialog(this);
    if(Dialog.exec() == QDialog::Accepted)
    {
        if(_Interpreter != 0)
        {
            if(!_Interpreter->IsPrinting())
            {
                QMessageBox Msg(QMessageBox::Information, "A Print Is Currently Loaded!",
                                "If you continue, your current print will be unloaded from the interpreter and you will need to re-load it.\nDo you want to save the new settings?",
                                QMessageBox::Yes, this);
                Msg.addButton(QMessageBox::No);
                if(Msg.exec() == QMessageBox::Yes)
                {
                    _Interpreter->TerminatePrint();
                    _Interpreter->wait();
                    delete _Interpreter;
                    _Interpreter = 0;
                }
                else
                   return;
            }
            UpdateMotorSettings();
        }
        else
            UpdateMotorSettings();
    }
}
void MainWindow::on_actionConfigure_Probes_triggered()
{
    ProbeConfigDialog Dialog(this);
    Dialog.exec();
}
void MainWindow::on_actionSetup_Print_Area_triggered()
{
    PrintAreaConfigDialog Dialog(this);
    Dialog.exec();
}
void MainWindow::on_actionS_tart_triggered()
{
   if(_Interpreter != 0)
   {
       if(_Interpreter->IsPrinting())
       {
           if(!_Interpreter->IsPaused())
           {
               _Interpreter->PausePrint();
               ui->menuPrint_Actions->actions()[0]->setText("Resume");
           }
           else
           {
               _Interpreter->PausePrint();
               ui->menuPrint_Actions->actions()[0]->setText("Pause");
           }
       }
       else
       {
           //Connect the probe reads to the lcd displays
           connect(_Interpreter->BedProbeWorker, SIGNAL(ReportTemp(int)), ui->lcdBedTemp, SLOT(display(int)));
           connect(_Interpreter->ExtProbeWorker, SIGNAL(ReportTemp(int)), ui->lcdExtruderTemp, SLOT(display(int)));

           //connect the Probe element states to the UI
           connect(_Interpreter->BedProbeWorker, SIGNAL(ReportElementState(bool)), ui->lblBedOn, SLOT(setVisible(bool)));
           connect(_Interpreter->ExtProbeWorker, SIGNAL(ReportElementState(bool)), ui->lblExtOn, SLOT(setVisible(bool)));

           //Connect the Temp Overrides
           connect(ui->seBed, SIGNAL(valueChanged(int)), _Interpreter, SLOT(ChangeBedTemp(int)));
           connect(ui->seExt, SIGNAL(valueChanged(int)), _Interpreter, SLOT(ChangeExtTemp(int)));

           //Record the GCODE temp and set it back once the override is turned off.
           connect(ui->chkBedOverride, SIGNAL(toggled(bool)), this, SLOT(on_BedTempOverride(bool)));
           connect(ui->chkExtOverride, SIGNAL(toggled(bool)), this, SLOT(on_ExtTempOverride(bool)));

           //Connect the interpreter feedback to the ui controls
           connect(_Interpreter, SIGNAL(ReportProgress(int)), this->_ProgressBar, SLOT(setValue(int)));
           connect(_Interpreter, SIGNAL(BeginLineProcessing(QString)), ui->txtGCode, SLOT(append(QString)));
           connect(_Interpreter, SIGNAL(ProcessingTemps(QString)), this->_StatusLabel, SLOT(setText(QString)));
           connect(_Interpreter, SIGNAL(ProcessingMoves(QString)), this->_StatusLabel, SLOT(setText(QString)));
           connect(_Interpreter, SIGNAL(PrintComplete()), this, SLOT(on_action_Stop_triggered()));
           connect(_Interpreter, SIGNAL(OnError(QString,QString)), this, SLOT(DisplayError(QString,QString)));
           connect(_Interpreter, SIGNAL(ReportMotorPosition(QString,long)), this, SLOT(UpdatePositionLabel(QString,long)));

           _Interpreter->start();//Kick the tires and light the fires...

           ui->menuPrint_Actions->actions()[0]->setText("Pause");
       }
   }
   else
   {
        QMessageBox Msg(this);
        Msg.setText("A Print has not been loaded yet!");
        Msg.exec();
   }
}
void MainWindow::on_action_Stop_triggered()
{
    if(_Interpreter != 0)
    {
        _Interpreter->TerminatePrint();
        _Interpreter->wait();
        delete _Interpreter;
        _Interpreter = 0;
        ui->menuPrint_Actions->actions()[0]->setText("S&tart");
        _ProgressBar->setValue(0);
        _StatusLabel->setText("Ready");
        ui->txtGCode->clear();
        ui->lcdBedTemp->display("Off");
        ui->lcdExtruderTemp->display("Off");
    }
}
void MainWindow::on_BedTempOverride(bool Arg)
{
    if(!Arg && _Interpreter != 0)
        ui->seBed->setValue(_Interpreter->GetBedGcodeTemp());
}
void MainWindow::on_ExtTempOverride(bool Arg)
{
    if(!Arg && _Interpreter != 0)
        ui->seExt->setValue(_Interpreter->GetExtGcodeTemp());
}
//==============End Main Menu Handlers=================================================
void MainWindow::UpdateMotorSettings()
{
    InitializeMotors();
}
void MainWindow::UpdatePositionLabel(QString Name, const long Pos)
{
    if(Name == "XAxis")
        ui->lblXPos->setText(QString::number(Pos));
    else if(Name == "YAxis")
        ui->lblYPos->setText(QString::number(Pos));
    else if(Name == "ZAxis")
        ui->lblZPos->setText(QString::number(Pos));
    else if(Name == "ExtAxis")
        ui->lblExtPos->setText(QString::number(Pos));
}

void MainWindow::on_btnXLeft_pressed()
{
    _XWorker->StopThread = false;
    _XAxis->Rotate(StepperMotor::CTRCLOCKWISE, 1, _XAxis->MaxSpeed());
    UpdatePositionLabel(QString::fromStdString(_XAxis->MotorName),_XAxis->Position);
}
void MainWindow::on_btnXLeft_released()
{
    _XWorker->Stop();
}

void MainWindow::on_btnXRight_pressed()
{
    _XWorker->StopThread = false;
    _XAxis->Rotate(StepperMotor::CLOCKWISE, 1, _XAxis->MaxSpeed());
    UpdatePositionLabel(QString::fromStdString(_XAxis->MotorName),_XAxis->Position);
}
void MainWindow::on_btnXRight_released()
{
        _XWorker->Stop();
}

void MainWindow::on_btnYBack_pressed()
{
    _YWorker->StopThread = false;
    _YAxis->Rotate(StepperMotor::CTRCLOCKWISE, 1, _YAxis->MaxSpeed());
    UpdatePositionLabel(QString::fromStdString(_YAxis->MotorName), _YAxis->Position);
}

void MainWindow::on_btnYBack_released()
{
        _YWorker->Stop();
}

void MainWindow::on_btnYFore_pressed()
{
    _YWorker->StopThread = false;
    _YAxis->Rotate(StepperMotor::CLOCKWISE, 1, _YAxis->MaxSpeed());
    UpdatePositionLabel(QString::fromStdString(_YAxis->MotorName), _YAxis->Position);
}

void MainWindow::on_btnYFore_released()
{
        _YWorker->Stop();
}

void MainWindow::on_btnZUp_pressed()
{
    _ZWorker->StopThread = false;
    _ZAxis->Rotate(StepperMotor::CTRCLOCKWISE, 1, _ZAxis->MaxSpeed());
    UpdatePositionLabel(QString::fromStdString(_ZAxis->MotorName), _ZAxis->Position);
}

void MainWindow::on_btnZUp_released()
{
        _ZWorker->Stop();
}

void MainWindow::on_btnZDown_pressed()
{
    _ZWorker->StopThread = false;
    _ZAxis->Rotate(StepperMotor::CLOCKWISE, 1, _ZAxis->MaxSpeed());
    UpdatePositionLabel(QString::fromStdString(_ZAxis->MotorName), _ZAxis->Position);
}

void MainWindow::on_btnZDown_released()
{
        _ZWorker->Stop();
}

void MainWindow::on_btnEXTFore_pressed()
{
    _ExtWorker->StopThread = false;
    _ExtAxis->Rotate(StepperMotor::CLOCKWISE, 1, _ExtAxis->MaxSpeed());
    UpdatePositionLabel(QString::fromStdString(_ExtAxis->MotorName), _ExtAxis->Position);
}

void MainWindow::on_btnEXTFore_released()
{
        _ExtWorker->Stop();
}

void MainWindow::on_btnEXTBack_pressed()
{
    _ExtWorker->StopThread = false;
    _ExtAxis->Rotate(StepperMotor::CTRCLOCKWISE, 1, _ExtAxis->MaxSpeed());
    UpdatePositionLabel(QString::fromStdString(_ExtAxis->MotorName), _ExtAxis->Position);
}

void MainWindow::on_btnEXTBack_released()
{
            _ExtWorker->Stop();
}
//==============END SLOTS============================================================




