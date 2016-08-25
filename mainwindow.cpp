#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stepperdriver.h"
#include "motorconfigdialog.h"
#include "gcodeinterpreter.h"

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

    ui->statusBar->insertPermanentWidget(0, _ProgressBar, 100);
    ui->statusBar->insertPermanentWidget(1, _StatusLabel, 250);

    _Interpreter = new GCodeInterpreter("","");

	//TODO:Connect up the Interpreter's signals and slots before moving it to a thread and 
	//beginning the print somewhere in the class.
}

MainWindow::~MainWindow()
{
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

//==========================End Private Methods======================================
//==========================Main Menu Handlers=======================================
void MainWindow::on_action_Stepper_Utility_triggered()
{
    StepperDriver *SD = new StepperDriver(this);
    SD->show();
}

void MainWindow::on_action_Exit_triggered()
{
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
        delete _Interpreter;
        _Interpreter = new GCodeInterpreter(_PrintFilePath, "LOG");
        connect(_Interpreter->BedProbe, SIGNAL(ReportTemp(int)), ui->lcdBedTemp, SLOT(display(int)));
        connect(_Interpreter->ExtProbe, SIGNAL(ReportTemp(int)), ui->lcdExtruderTemp, SLOT(display(int)));
    }
}
void MainWindow::on_action_Configure_PiRinter_triggered()
{
    MotorConfigDialog Dialog;
    Dialog.exec();
}
//==============End Main Menu Handlers================================================

//==============SLOTS===============================================================

//==============END SLOTS============================================================

