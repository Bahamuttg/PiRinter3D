#include "motorconfigdialog.h"
#include "ui_motorconfigdialog.h"
#include <QMessageBox>
MotorConfigDialog::MotorConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotorConfigDialog)
{
    ui->setupUi(this);
    ui->lblEnableX->setHidden(true);
    ui->leEnableX->setHidden(true);
    ui->lblEnableY->setHidden(true);
    ui->leEnableY->setHidden(true);
    ui->lblEnableZ->setHidden(true);
    ui->leEnableZ->setHidden(true);
    ui->lblEnableEX->setHidden(true);
    ui->leEnableEX->setHidden(true);
    ui->tpXAxis->setFocus();
    ReadCfgFile();
}

MotorConfigDialog::~MotorConfigDialog()
{
    delete ui;
}

void MotorConfigDialog::on_buttonBox_accepted()
{
    this->WriteCfgFile();
    emit OnConfigChanged();
}

void MotorConfigDialog::WriteCfgFile()
{
    QFile MotorCfg("MotorCfg.ini");
    if(MotorCfg.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QTextStream OutStream(&MotorCfg);

        OutStream << "MotorConfig::XAxis;" << ui->leA1_X->text() << ";" << ui->leA2_X->text() << ";" << ui->leB1_X->text() << ";" << ui->leB2_X->text() << ";" << ui->leEnableX->text() << ";";
        OutStream << ui->chkNOTX->isChecked() << ";" << ui->chkEnableX->isChecked() << ";" << ui->rbFullX->isChecked() << ";" << ui->rbHalfX->isChecked() << ";" << ui->dsbResX->value()  <<"\n";

        OutStream << "MotorConfig::YAxis;" << ui->leA1_Y->text() << ";" << ui->leA2_Y->text() << ";" << ui->leB1_Y->text() << ";" << ui->leB2_Y->text() << ";" << ui->leEnableY->text() << ";";
        OutStream << ui->chkNOTY->isChecked() << ";" << ui->chkEnableY->isChecked() << ";" << ui->rbFullY->isChecked() << ";" << ui->rbHalfY->isChecked() << ";" << ui->dsbResY->value()  <<"\n";

        OutStream << "MotorConfig::ZAxis;" << ui->leA1_Z->text() << ";" << ui->leA2_Z->text() << ";" << ui->leB1_Z->text() << ";" << ui->leB2_Z->text() << ";" << ui->leEnableZ->text() << ";";
        OutStream << ui->chkNOTZ->isChecked() << ";" << ui->chkEnableZ->isChecked() << ";" << ui->rbFullZ->isChecked() << ";" << ui->rbHalfZ->isChecked() << ";" << ui->dsbResZ->value()  <<"\n";

        OutStream << "MotorConfig::ExtAxis;" << ui->leA1_EX->text() << ";" << ui->leA2_EX->text() << ";" << ui->leB1_EX->text() << ";" << ui->leB2_EX->text() << ";" << ui->leEnableEX->text() << ";";
        OutStream << ui->chkNOTEX->isChecked() << ";" << ui->chkEnableEX->isChecked() << ";" << ui->rbFullEX->isChecked() << ";" << ui->rbHalfEX->isChecked() << ";" << ui->dsbResEX->value()  <<"\n";
        OutStream.flush();
        MotorCfg.close();
    }
    else
        QMessageBox::critical(this, "Error Opening File!", MotorCfg.errorString(), QMessageBox::Ok);
}

void MotorConfigDialog::ReadCfgFile()
{
    QFile MotorCfg("MotorCfg.ini");
    if(MotorCfg.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        QTextStream InStream(&MotorCfg);
        while(!InStream.atEnd())
        {
            QString Line = InStream.readLine(); //read one line at a time
            if(Line.contains("MotorConfig"))
            {
                QStringList Params = Line.split(";");
                if(Params[0].contains("XAxis") && Params.count() == 11)
                {
                    ui->leA1_X->setText(Params[1]);
                    ui->leA2_X->setText(Params[2]);
                    ui->leB1_X->setText(Params[3]);
                    ui->leB2_X->setText(Params[4]);
                    ui->leEnableX->setText(Params[5]);
                    ui->chkNOTX->setChecked(Params[6].toInt());
                    ui->chkEnableX->setChecked(Params[7].toInt());
                    ui->rbFullX->setChecked(Params[8].toInt());
                    ui->rbHalfX->setChecked(Params[9].toInt());
                    ui->dsbResX->setValue(Params[10].toDouble());
                }
                if(Params[0].contains("YAxis") && Params.count() == 11)
                {
                    ui->leA1_Y->setText(Params[1]);
                    ui->leA2_Y->setText(Params[2]);
                    ui->leB1_Y->setText(Params[3]);
                    ui->leB2_Y->setText(Params[4]);
                    ui->leEnableY->setText(Params[5]);
                    ui->chkNOTY->setChecked(Params[6].toInt());
                    ui->chkEnableY->setChecked(Params[7].toInt());
                    ui->rbFullY->setChecked(Params[8].toInt());
                    ui->rbHalfY->setChecked(Params[9].toInt());
                    ui->dsbResY->setValue(Params[10].toDouble());
                }
                if(Params[0].contains("ZAxis") && Params.count() == 11)
                {
                    ui->leA1_Z->setText(Params[1]);
                    ui->leA2_Z->setText(Params[2]);
                    ui->leB1_Z->setText(Params[3]);
                    ui->leB2_Z->setText(Params[4]);
                    ui->leEnableZ->setText(Params[5]);
                    ui->chkNOTZ->setChecked(Params[6].toInt());
                    ui->chkEnableZ->setChecked(Params[7].toInt());
                    ui->rbFullZ->setChecked(Params[8].toInt());
                    ui->rbHalfZ->setChecked(Params[9].toInt());
                    ui->dsbResZ->setValue(Params[10].toDouble());
                }
                if(Params[0].contains("ExtAxis") && Params.count() == 11)
                {
                    ui->leA1_EX->setText(Params[1]);
                    ui->leA2_EX->setText(Params[2]);
                    ui->leB1_EX->setText(Params[3]);
                    ui->leB2_EX->setText(Params[4]);
                    ui->leEnableEX->setText(Params[5]);
                    ui->chkNOTEX->setChecked(Params[6].toInt());
                    ui->chkEnableEX->setChecked(Params[7].toInt());
                    ui->rbFullEX->setChecked(Params[8].toInt());
                    ui->rbHalfEX->setChecked(Params[9].toInt());
                    ui->dsbResEX->setValue(Params[10].toDouble());
                }
            }
            MotorCfg.close();
        }
    }
    else
        QMessageBox::critical(this, "Error Opening File!", MotorCfg.errorString(), QMessageBox::Ok);
}
