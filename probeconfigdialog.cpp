#include "probeconfigdialog.h"
#include "ui_probeconfigdialog.h"

ProbeConfigDialog::ProbeConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProbeConfigDialog)
{
    ui->setupUi(this);
}

ProbeConfigDialog::~ProbeConfigDialog()
{
    delete ui;
}
