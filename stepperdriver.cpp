#include "stepperdriver.h"
#include "ui_stepperdriver.h"
#include "motorworker.h"
#include "steppermotor.h"
#include <QtCore>
#include <QTimer>
#include <QThread>
#include <QDebug>

StepperDriver::StepperDriver(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StepperDriver)
{
    ui->setupUi(this);

    ButtonTimer = new QTimer(this);
    MotorThread  = 0;
    Motor = 0;
    Worker = 0;

    ButtonTimer->setInterval(300);

    connect (ui->rbX, SIGNAL(toggled(bool)), this, SLOT(UpdateMotorSettings()));
    connect (ui->rbY, SIGNAL(toggled(bool)), this, SLOT(UpdateMotorSettings()));
    connect (ui->rbX, SIGNAL(toggled(bool)), this, SLOT(UpdateMotorSettings()));
    connect (ui->rbExt, SIGNAL(toggled(bool)), this, SLOT(UpdateMotorSettings()));

    //Link the Timer elapsed to the Timer stop so we can use it again.
    connect(ButtonTimer, SIGNAL(timeout()), ButtonTimer, SLOT(stop()));
    //Link the Buttons pressed signal to the timer start
    connect(ui->pushButton, SIGNAL(pressed()), ButtonTimer, SLOT(start()));
    connect(ui->pushButton_2, SIGNAL(pressed()), ButtonTimer, SLOT(start()));

    //Link the Buttons released signal to the ButtonTimer stop.
    connect(ui->pushButton, SIGNAL(released()), ButtonTimer, SLOT(stop()));
    connect(ui->pushButton_2, SIGNAL(released()), ButtonTimer, SLOT(stop()));
    //Setup a default motor;
    UpdateMotorSettings();
}

StepperDriver::~StepperDriver()
{
    delete ui;
    delete MotorThread;
    delete ButtonTimer;
    delete Motor;
}

/*
 *  Param 0: Motor Name
 *  Param 1: CoilA1 GPIO Pin
 *  Param 2: CoilA2 GPIO Pin
 *  Param 3: CoilB1 GPIO Pin
 *  Param 4: CoilB2 GPIO Pin
 *  Param 5: End Stop GPIO Pin
 *  Param 6: Hex Inverter Flag
 *  Param 7: End Stop Flag
 *  Param 8: Full Stepping Flag
 *  Param 9: Half Stepping Flag
 *  Param 10: Resolution Value
 *  Param 11: Min Phase Delay Value
 *  Param 12: Has Enable Flag
 *  Param 13: External Ctrl Step GPIO Pin
 *  Param 14: External Ctrl Direction GPIO Pin
 *  Param 15: Enable GPIO Pin
 *  Param 16: Use External Ctrl Flag
 *  Param 17: Hold On Idle Flag
 */

void StepperDriver::InitializeMotor(const QString &MotorName)
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
                if(Params[0].contains(MotorName))
                {
                    if(Params[6].toInt()) //If it's using HEX inverters
                        this->Motor = new StepperMotor(Params[1].toInt(), Params[3].toInt(), Params[11].toInt(), Params[0].split("::")[1].toStdString(), Params[5].toInt());
                    else if(Params[16].toInt())
                        this->Motor = new StepperMotor(Params[13].toInt(), Params[14].toInt(), Params[15].toInt(), Params[11].toInt(),  Params[0].split("::")[1].toStdString(), Params[5].toInt());
                    else
                        this->Motor = new StepperMotor(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(), Params[11].toInt(),
                            Params[9].toInt(), Params[0].split("::")[1].toStdString(), Params[5].toInt());
                    this->Motor->SetHoldOnIdle(Params[17].toInt());
                }
            }
        }
        MotorConfig.close();
    }
    else
    {
        QMessageBox::critical(this, "Error Opening File!", MotorConfig.errorString(), QMessageBox::Ok);
        this->close();
    }
}

void StepperDriver::UpdateMotorSettings()
{
    if (Worker != 0 && Worker->IsBusy)
        Worker->Stop();

    if(MotorThread != 0)
        delete MotorThread;
    if(Worker != 0)
        delete Worker;
    if(Motor != 0)
        delete Motor;

    MotorThread = new QThread(this);

    //This logic loop sets up the motor based upon the radio box value;
    if(ui->rbX->isChecked())
        InitializeMotor("XAxis");
    else if(ui->rbY->isChecked())
        InitializeMotor("YAxis");
    else if (ui->rbZ->isChecked())
        InitializeMotor("ZAxis");
    else if(ui->rbExt->isChecked())
        InitializeMotor("ExtAxis");

    Worker = new MotorWorker(Motor);
    Worker->moveToThread(MotorThread);

    //Link the MotorThread to the DoWork Slot.
    connect(MotorThread, SIGNAL(started()), Worker, SLOT(DoWork()));
    connect(MotorThread, SIGNAL(terminated()), Worker, SLOT(Stop()));
    connect(Worker, SIGNAL(ProgressChanged(QString)), this, SLOT(UpdatePositionLabel(QString)));
    //Link the Timer elapsed to the MotorThread start.
    connect(ButtonTimer, SIGNAL(timeout()), MotorThread, SLOT(start()));
    //Link the Buttons released signal to the MotorThread quit.
    connect(ui->pushButton, SIGNAL(released()), MotorThread, SLOT(quit()));
    connect(ui->pushButton_2, SIGNAL(released()), MotorThread, SLOT(quit()));
}

void StepperDriver::on_pushButton_2_pressed()
{
    Worker->StopThread = false;
    Motor->Rotate(StepperMotor::CTRCLOCKWISE, 1, Motor->MaxSpeed());
    UpdatePositionLabel(QString::number(Motor->Position));
}

void StepperDriver::on_pushButton_pressed()
{
    Worker->StopThread = false;
    Motor->Rotate(StepperMotor::CLOCKWISE, 1, Motor->MaxSpeed());
    UpdatePositionLabel(QString::number(Motor->Position));
}

void StepperDriver::errorString(QString err)
{
    qDebug() << err;
}

void StepperDriver::on_pushButton_released()
{
    Worker->Stop();
}

void StepperDriver::on_pushButton_2_released()
{
    Worker->Stop();
}

void StepperDriver::UpdatePositionLabel(QString Arg)
{
    ui->lblMotorPosition->setText(Arg);
}

void StepperDriver::UpdateLabels()
{
    ui->lblMotorPosition->setText("0");
}


//void StepperDriver::on_action_Reset_MotorPosition_triggered()
//{
//    this->Motor->Position = 0;
//    ui->lblMotorPosition->setText("0");
//}
