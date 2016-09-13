#include "stepperdriver.h"
#include "ui_stepperdriver.h"
#include "motorworker.h"
#include "steppermotor.h"
#include <QtCore>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include "coildialog.h"

StepperDriver::StepperDriver(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StepperDriver)
{
	ui->setupUi(this);

	ButtonTimer = new QTimer(this);
	MotorThread = new QThread(this);
    Motor_1 = new StepperMotor(_A1, _A2, _B1, _B2, 5);
	Worker = new MotorWorker(Motor_1);
	_UseHexInverter = false;
	_A1 = 0;
	_A2 = 0;
	_B1 = 0;
	_B2 = 0;
	ButtonTimer->setInterval(1000);

	//Link the Timer elapsed to the Timer stop so we can use it again.
	connect(ButtonTimer, SIGNAL(timeout()), ButtonTimer, SLOT(stop()));
	//Link the Buttons pressed signal to the timer start
	connect(ui->pushButton, SIGNAL(pressed()), ButtonTimer, SLOT(start()));
	connect(ui->pushButton_2, SIGNAL(pressed()), ButtonTimer, SLOT(start()));

	//Link the Buttons released signal to the ButtonTimer stop.
	connect(ui->pushButton, SIGNAL(released()), ButtonTimer, SLOT(stop()));
	connect(ui->pushButton_2, SIGNAL(released()), ButtonTimer, SLOT(stop()));
}

StepperDriver::~StepperDriver()
{
	delete ui;
	delete MotorThread;
	delete ButtonTimer;
	delete Motor_1;
}

void StepperDriver::on_pushButton_2_pressed()
{
	Worker->StopThread = false;
    Motor_1->Rotate(StepperMotor::CTRCLOCKWISE, 1, 5);
	UpdatePositionLabel(QString::number(Motor_1->Position));
	qDebug() << "Rotating Counter Clockwise!";
}

void StepperDriver::on_pushButton_pressed()
{
	Worker->StopThread = false;
    Motor_1->Rotate(StepperMotor::CLOCKWISE, 1, 5);
	UpdatePositionLabel(QString::number(Motor_1->Position));
	qDebug() << "Rotating Clockwise!";
}

void StepperDriver::ResetThreadStop()
{
	Worker->StopThread = false;
	this->StopThread = false;
}

void StepperDriver::errorString(QString err)
{
	qDebug() << err;
}

void StepperDriver::on_pushButton_released()
{
	qDebug() << "Button Released!";
	Worker->Stop();
}

void StepperDriver::on_pushButton_2_released()
{
	qDebug() << "Button Released!";
	Worker->Stop();
}

void StepperDriver::UpdatePositionLabel(QString Arg)
{
    ui->lblMotorPosition->setText(Arg);
}

void StepperDriver::UpdateLabels()
{
	ui->lcdA1->display((int)_A1);
	ui->lcdA2->display((int)_A2);
	ui->lcdB1->display((int)_B1);
	ui->lcdB2->display((int)_B2);
	ui->lblMotorPosition->setText("0");
}

void StepperDriver::UpdateMotorSettings()
{
	if (Worker->IsBusy)
		Worker->Stop();

	delete Motor_1;
	delete Worker;
	delete MotorThread;

	MotorThread = new QThread(this);   
    Motor_1 = new StepperMotor(_A1, _A2, _B1, _B2, false);
    Motor_1->SetNotGated(_UseHexInverter);
    Worker = new MotorWorker(Motor_1);
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

void StepperDriver::on_action_Exit_triggered()
{
	this->close();
}

void StepperDriver::on_action_Configure_Coils_triggered()
{
	CoilDialog *CD = new CoilDialog(_A1, _A2, _B1, _B2, this);
	CD->setModal(true);
	CD->exec();
	this->_A1 = CD->A1;
	this->_A2 = CD->A2;
	this->_B1 = CD->B1;
	this->_B2 = CD->B2;
	delete CD;
	UpdateLabels();
    UpdateMotorSettings();
}

void StepperDriver::on_actionUse_NOT_Gates_toggled(bool arg1)
{
	this->_UseHexInverter = arg1;
}

void StepperDriver::on_action_Reset_MotorPosition_triggered()
{
	this->Motor_1->Position = 0;
	ui->lblMotorPosition->setText("0");
}

void StepperDriver::on_action_Turn_Off_Coils_triggered()
{
	this->Motor_1->Disable();
}

void StepperDriver::on_action_Enable_Motor_triggered()
{
	this->Motor_1->Enable();
}
