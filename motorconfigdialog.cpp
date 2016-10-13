#include "motorconfigdialog.h"
#include "ui_motorconfigdialog.h"
#include <QMessageBox>
MotorConfigDialog::MotorConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotorConfigDialog)
{
    ui->setupUi(this);
    ui->lblEndX->setHidden(true);
    ui->leEndX->setHidden(true);
    ui->lblEndY->setHidden(true);
    ui->leEndY->setHidden(true);
    ui->lblEndZ->setHidden(true);
    ui->leEndZ->setHidden(true);
    ui->lblEndEX->setHidden(true);
    ui->leEndEX->setHidden(true);

    ui->leStepPinX->setHidden(true);
    ui->lblStepX->setHidden(true);
    ui->leDirPinX->setHidden(true);
    ui->lblDirX->setHidden(true);

    ui->leEnablePinX->setHidden(true);
    ui->lblEnableX->setHidden(true);


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
     */

    QFile MotorCfg("MotorCfg.ini");
    if(MotorCfg.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {   
        QTextStream OutStream(&MotorCfg);

        OutStream << "MotorConfig::XAxis;" << ui->leA1_X->text() << ";" << ui->leA2_X->text() << ";" << ui->leB1_X->text() << ";" << ui->leB2_X->text() << ";" << ui->leEndX->text() << ";";
        OutStream << ui->chkNOTX->isChecked() << ";" << ui->chkEndX->isChecked() << ";" << ui->rbFullX->isChecked() << ";" << ui->rbHalfX->isChecked() << ";" << ui->dsbResX->value()  << ";";
        OutStream << ui->seMinDelayX->value() << ";"  << ui->chkEnableX->isChecked() << ";" << ui->leStepPinX->text() << ";" << ui->leDirPinX->text() << ";" << ui->leEnablePinX->text() << ";" << ui->chkExtCtrlX->isChecked()<< "\n";

        OutStream << "MotorConfig::YAxis;" << ui->leA1_Y->text() << ";" << ui->leA2_Y->text() << ";" << ui->leB1_Y->text() << ";" << ui->leB2_Y->text() << ";" << ui->leEndY->text() << ";";
        OutStream << ui->chkNOTY->isChecked() << ";" << ui->chkEndY->isChecked() << ";" << ui->rbFullY->isChecked() << ";" << ui->rbHalfY->isChecked() << ";" << ui->dsbResY->value() << ";" << ui->seMinDelayY->value()   <<"\n";

        OutStream << "MotorConfig::ZAxis;" << ui->leA1_Z->text() << ";" << ui->leA2_Z->text() << ";" << ui->leB1_Z->text() << ";" << ui->leB2_Z->text() << ";" << ui->leEndZ->text() << ";";
        OutStream << ui->chkNOTZ->isChecked() << ";" << ui->chkEndZ->isChecked() << ";" << ui->rbFullZ->isChecked() << ";" << ui->rbHalfZ->isChecked() << ";" << ui->dsbResZ->value()<< ";" << ui->seMinDelayZ->value()   <<"\n";

        OutStream << "MotorConfig::ExtAxis;" << ui->leA1_EX->text() << ";" << ui->leA2_EX->text() << ";" << ui->leB1_EX->text() << ";" << ui->leB2_EX->text() << ";" << ui->leEndEX->text() << ";";
        OutStream << ui->chkNOTEX->isChecked() << ";" << ui->chkEndEX->isChecked() << ";" << ui->rbFullEX->isChecked() << ";" << ui->rbHalfEX->isChecked() << ";" << ui->dsbResEX->value() << ";" << ui->seMinDelayExt->value()  <<"\n";
        OutStream.flush();
        MotorCfg.close();
    }
    else
        QMessageBox::critical(this, "Error Opening File!", MotorCfg.errorString(), QMessageBox::Ok);
}

void MotorConfigDialog::ReadCfgFile()
{
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
     */

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
                if( Params[0].contains("XAxis") && Params.count() == 17)
                {
                    ui->leA1_X->setText(Params[1]);
                    ui->leA2_X->setText(Params[2]);
                    ui->leB1_X->setText(Params[3]);
                    ui->leB2_X->setText(Params[4]);
                    ui->leEndX->setText(Params[5]);

                    ui->chkNOTX->setChecked(Params[6].toInt());
                    ui->chkEndX->setChecked(Params[7].toInt());
                    ui->rbFullX->setChecked(Params[8].toInt());
                    ui->rbHalfX->setChecked(Params[9].toInt());
                    ui->dsbResX->setValue(Params[10].toDouble());

                    ui->seMinDelayX->setValue(Params[11].toInt());
                    ui->chkEnableX->setChecked(Params[12].toInt());
                    ui->leStepPinX->setText(Params[13]);
                    ui->leDirPinX->setText(Params[14]);
                    ui->leEnablePinX->setText(Params[15]);
                    ui->chkExtCtrlX->setChecked(Params[16].toInt());
                }
                if(Params[0].contains("YAxis") && Params.count() == 17)
                {
                    ui->leA1_Y->setText(Params[1]);
                    ui->leA2_Y->setText(Params[2]);
                    ui->leB1_Y->setText(Params[3]);
                    ui->leB2_Y->setText(Params[4]);
                    ui->leEndY->setText(Params[5]);

                    ui->chkNOTY->setChecked(Params[6].toInt());
                    ui->chkEndY->setChecked(Params[7].toInt());
                    ui->rbFullY->setChecked(Params[8].toInt());
                    ui->rbHalfY->setChecked(Params[9].toInt());
                    ui->dsbResY->setValue(Params[10].toDouble());
                    ui->seMinDelayY->setValue(Params[11].toInt());
                }
                if(Params[0].contains("ZAxis") && Params.count() == 17)
                {
                    ui->leA1_Z->setText(Params[1]);
                    ui->leA2_Z->setText(Params[2]);
                    ui->leB1_Z->setText(Params[3]);
                    ui->leB2_Z->setText(Params[4]);
                    ui->leEndZ->setText(Params[5]);
                    ui->chkNOTZ->setChecked(Params[6].toInt());
                    ui->chkEndZ->setChecked(Params[7].toInt());
                    ui->rbFullZ->setChecked(Params[8].toInt());
                    ui->rbHalfZ->setChecked(Params[9].toInt());
                    ui->dsbResZ->setValue(Params[10].toDouble());
                    ui->seMinDelayZ->setValue(Params[11].toInt());
                }
                if(Params[0].contains("ExtAxis") && Params.count() == 17)
                {
                    ui->leA1_EX->setText(Params[1]);
                    ui->leA2_EX->setText(Params[2]);
                    ui->leB1_EX->setText(Params[3]);
                    ui->leB2_EX->setText(Params[4]);
                    ui->leEndEX->setText(Params[5]);
                    ui->chkNOTEX->setChecked(Params[6].toInt());
                    ui->chkEndEX->setChecked(Params[7].toInt());
                    ui->rbFullEX->setChecked(Params[8].toInt());
                    ui->rbHalfEX->setChecked(Params[9].toInt());
                    ui->dsbResEX->setValue(Params[10].toDouble());
                    ui->seMinDelayExt->setValue(Params[11].toInt());
                }
            }
            MotorCfg.close();
        }
    }
    else
        QMessageBox::critical(this, "Error Opening File!", MotorCfg.errorString(), QMessageBox::Ok);
}

void MotorConfigDialog::on_chkExtCtrlX_toggled(bool checked)
{
    ui->chkNOTX->setChecked(false);
    ui->chkNOTX->setEnabled(!checked);

    ui->leA1_X->setHidden(checked);
    ui->leA2_X->setHidden(checked);
    ui->leB1_X->setHidden(checked);
    ui->leB2_X->setHidden(checked);
    ui->lblA1X->setHidden(checked);
    ui->lblA2X->setHidden(checked);
    ui->lblB1X->setHidden(checked);
    ui->lblB2X->setHidden(checked);

    ui->leStepPinX->setVisible(checked);
    ui->lblStepX->setVisible(checked);
    ui->leDirPinX->setVisible(checked);
    ui->lblDirX->setVisible(checked);
}
