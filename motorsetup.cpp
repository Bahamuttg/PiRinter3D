#include "motorsetup.h"
#include "ui_motorsetup.h"

MotorSetup::MotorSetup(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::MotorSetup)
{
    ui->setupUi(this);
}

MotorSetup::~MotorSetup()
{
    delete ui;
}
