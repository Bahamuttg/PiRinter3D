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

    ButtonTimer = new QTimer(this);
    ButtonTimer->setInterval(300);

    _XWorker = 0;
    _YWorker = 0;
    _ZWorker = 0;
    _ExtWorker = 0;

    _XMotorThread = 0;
    _YMotorThread = 0;
    _ZMotorThread = 0;
    _ExtMotorThread = 0;

    _Interpreter = 0;
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

    delete ui;
}
//==========================Private Methods===========================================
void MainWindow::LoadConfigurations()
{
    //TODO Load Configs.
}

void MainWindow::SaveConfigurations()
{
    //TODO Save Configs.
}

//==========================End Private Methods========================================

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
        UpdateMotorSettings();
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
           connect(_Interpreter->BedProbeWorker, SIGNAL(ReportElementState(bool)), ui->chkBedOn, SLOT(setChecked(bool)));
           connect(_Interpreter->ExtProbeWorker, SIGNAL(ReportElementState(bool)), ui->chkExtOn, SLOT(setChecked(bool)));

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
           //connect(_Interpreter, SIGNAL(MoveComplete(QString)), this->_StatusLabel, SLOT(setText(QString)));
           connect(_Interpreter, SIGNAL(ProcessingMoves(QString)), this->_StatusLabel, SLOT(setText(QString)));
           connect(_Interpreter, SIGNAL(PrintComplete()), this, SLOT(on_action_Stop_triggered()));
           connect(_Interpreter, SIGNAL(OnError(QString,QString)), this, SLOT(DisplayError(QString,QString)));
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
void MainWindow::UpdatePositionLabel(QString Value)
{
}
//==============END SLOTS============================================================

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
