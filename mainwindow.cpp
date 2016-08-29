#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stepperdriver.h"
#include "motorconfigdialog.h"
#include "gcodeinterpreter.h"
#include "probeconfigdialog.h"
#include "printareaconfigdialog.h"

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

    _Interpreter = 0;
}

MainWindow::~MainWindow()
{
    if(_Interpreter != 0)
    {
        _Interpreter->terminate();
    }
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
void MainWindow::on_action_Exit_triggered()
{
    if(_Interpreter->IsPrinting())
        _Interpreter->TerminatePrint();
    _Interpreter->terminate();
    _Interpreter->wait();
    this->close();
}
void MainWindow::on_action_Load_3D_Print_triggered()
{
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
    Dialog.exec();
}
void MainWindow::on_actionConfigure_Temperatures_triggered()
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
           //connect(_Interpreter->BedProbeWorker, SIGNAL(ReportTemp(int)), ui->lcdBedTemp, SLOT(display(int)));
           //connect(_Interpreter->ExtProbeWorker, SIGNAL(ReportTemp(int)), ui->lcdExtruderTemp, SLOT(display(int)));

           //Connect the interpereter feedback to the ui controls
           connect(_Interpreter, SIGNAL(ReportProgress(int)), this->_ProgressBar, SLOT(setValue(int)));
           connect(_Interpreter, SIGNAL(BeginLineProcessing(QString)), ui->txtGCode, SLOT(append(QString)));
           connect(_Interpreter, SIGNAL(ProcessingTemps(QString)), this->_StatusLabel, SLOT(setText(QString)));
           connect(_Interpreter, SIGNAL(MoveComplete(QString)), this->_StatusLabel, SLOT(setText(QString)));
           connect(_Interpreter, SIGNAL(ProcessingMoves(QString)), this->_StatusLabel, SLOT(setText(QString)));
           connect(_Interpreter, SIGNAL(PrintComplete()), this, SLOT(on_action_Stop_triggered()));

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
        _Interpreter->terminate();
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
//==============End Main Menu Handlers=================================================
//==============END SLOTS============================================================




