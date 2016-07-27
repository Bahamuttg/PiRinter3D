#include "motorconfigdialog.h"
#include "ui_motorconfigdialog.h"

MotorConfigDialog::MotorConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotorConfigDialog)
{
    ui->setupUi(this);
}

MotorConfigDialog::~MotorConfigDialog()
{
    delete ui;
}
