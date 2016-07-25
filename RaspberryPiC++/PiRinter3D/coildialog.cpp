#include "coildialog.h"
#include "ui_coildialog.h"

CoilDialog::CoilDialog(int A1, int A2, int B1, int B2, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CoilDialog)
{
    ui->setupUi(this);
    ui->leA1->setFocus();
    connect(ui->leA1, SIGNAL(textChanged(QString)), this, SLOT(Value_ChangedA1(QString)));
    connect(ui->leA2, SIGNAL(textChanged(QString)), this, SLOT(Value_ChangedA2(QString)));
    connect(ui->leB1, SIGNAL(textChanged(QString)), this, SLOT(Value_ChangedB1(QString)));
    connect(ui->leB2, SIGNAL(textChanged(QString)), this, SLOT(Value_ChangedB2(QString)));
    ui->leA1->setText(QString::number(A1));
    ui->leA2->setText(QString::number(A2));
    ui->leB1->setText(QString::number(B1));
    ui->leB2->setText(QString::number(B2));
}

CoilDialog::~CoilDialog()
{
    delete ui;
}

void CoilDialog::on_buttonBox_accepted()
{
    A1 = ui->leA1->text().toInt();
    A2 = ui->leA2->text().toInt();
    B1 = ui->leB1->text().toInt();
    B2 = ui->leB2->text().toInt();
}

void CoilDialog::on_buttonBox_rejected()
{
    this->close();
}

void CoilDialog::Value_ChangedA1(const QString &Arg)
{
    this->A1 = Arg.toInt();
}
void CoilDialog::Value_ChangedA2(const QString &Arg)
{
    this->A2 = Arg.toInt();
}
void CoilDialog::Value_ChangedB1(const QString &Arg)
{
    this->B1 = Arg.toInt();
}
void CoilDialog::Value_ChangedB2(const QString &Arg)
{
    this->B2 = Arg.toInt();
}
