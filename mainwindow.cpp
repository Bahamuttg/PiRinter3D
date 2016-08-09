#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stepperdriver.h"
#include "motorconfigdialog.h"

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

    LoadConfigurations();
}

MainWindow::~MainWindow()
{
    delete ui;
}
//==========================Private Methods==========================================
void MainWindow::LoadConfigurations()
{
    //TODO Load Configs.
    InitializeMotors();
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
                    //TODO: check the not gate Param and invoke the NOT gate constructor if that is what we're using
                    this->XAxis = new StepperMotor(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(),
                            Params[5].toInt(), Params[0].split("::")[1].toStdString());
                    this->XAxis->SetNotGated(Params[6].toInt());
                }
                if(Params[0].contains("YAxis"))
                {
                    this->YAxis = new StepperMotor(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(),
                            Params[5].toInt(), Params[0].split("::")[1].toStdString());
                    this->YAxis->SetNotGated(Params[6].toInt());
                }
                if(Params[0].contains("ZAxis"))
                {
                    this->ZAxis = new StepperMotor(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(),
                            Params[5].toInt(), Params[0].split("::")[1].toStdString());
                    this->ZAxis->SetNotGated(Params[6].toInt());
                }
                if(Params[0].contains("ExtAxis"))
                {
                    this->ExtAxis = new StepperMotor(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(),
                            Params[5].toInt(), Params[0].split("::")[1].toStdString());
                    this->ExtAxis->SetNotGated(Params[6].toInt());
                }
            }
        }
    }
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
    }
}
void MainWindow::on_action_Configure_PiRinter_triggered()
{
    MotorConfigDialog Dialog;
    connect(&Dialog, SIGNAL(OnConfigChanged()), this, SLOT(UpdateMotorConfig()));
    Dialog.exec();
}
//==============End Main Menu Handlers================================================

//==============SLOTS===============================================================
void MainWindow::UpdateMotorConfig()
{
    //TODO Update the MotorCFG.ini;
}

void MainWindow::UpdateTempConfig()
{
    //TODO Update TempCFG.ini;
}

//==============END SLOTS============================================================

