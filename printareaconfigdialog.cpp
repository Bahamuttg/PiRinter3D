#include "printareaconfigdialog.h"
#include "ui_printareaconfigdialog.h"

PrintAreaConfigDialog::PrintAreaConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintAreaConfigDialog)
{
    ui->setupUi(this);
}

PrintAreaConfigDialog::~PrintAreaConfigDialog()
{
    delete ui;
}
