#include "probeconfigdialog.h"
#include "ui_probeconfigdialog.h"
#include <QMessageBox>

ProbeConfigDialog::ProbeConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProbeConfigDialog)
{
    ui->setupUi(this);
    ReadCfgFile();
}

ProbeConfigDialog::~ProbeConfigDialog()
{
    delete ui;
}

void ProbeConfigDialog::on_buttonBox_accepted()
{
    this->WriteCfgFile();
    emit OnConfigChanged();
}

void ProbeConfigDialog::WriteCfgFile()
{
    QFile ProbeCfg("ProbeCfg.ini");
    if(ProbeCfg.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QTextStream OutStream(&ProbeCfg);
        OutStream << "ProbeCfg::Bed;" << ui->seBedRefV->value() << ";" << ui->seBedR1Ohms->value() << ";" << ui->seBedBeta->value() << ";" << ui->seBedProbeOhms->value() << ";" << ui->seBedProbeTemp->value() << ";";
        OutStream << ui->txtBedElementPin->text() << ";" << ui->seBedADCCh->value() << ";" << ui->seBedPWM->value() <<"\n";
        OutStream << "ProbeCfg::Extruder;" << ui->seExtRefV->value() << ";" << ui->seExtR1Ohms->value() << ";" << ui->seExtBeta->value() << ";" << ui->seExtProbeOhms->value() << ";" << ui->seExtProbeTemp->value() << ";";
        OutStream << ui->txtExtElementPin->text() << ";" << ui->seExtADCCh->value() << ";" << ui->seExtPWM->value() <<"\n";
        OutStream << "ADCConfig::ADC1;" << ui->txtClock->text() << ";" << ui->txtMISO->text() << ";" << ui->txtMOSI->text() << ";" << ui->txtCS->text() << "\n";
        OutStream.flush();
        ProbeCfg.close();
    }
    else
        QMessageBox::critical(this, "Error Opening File!", ProbeCfg.errorString(), QMessageBox::Ok);
}

void ProbeConfigDialog::ReadCfgFile()
{
    QFile ProbeCfg("ProbeCfg.ini");
    if(ProbeCfg.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        QTextStream InStream(&ProbeCfg);
        while(!InStream.atEnd())
        {
            QString Line = InStream.readLine(); //read one line at a time
            if(Line.contains("ProbeCfg"))
            {
                QStringList Params = Line.split(";");
                if(Params[0].contains("Bed") && Params.count() == 9)
                {
                    ui->seBedRefV->setValue(Params[1].toDouble());
                    ui->seBedR1Ohms->setValue(Params[2].toInt());
                    ui->seBedBeta->setValue(Params[3].toInt());
                    ui->seBedProbeOhms->setValue(Params[4].toInt());
                    ui->seBedProbeTemp->setValue(Params[5].toDouble());
                    ui->txtBedElementPin->setText(Params[6]);
                    ui->seBedADCCh->setValue(Params[7].toInt());
                    ui->seBedPWM->setValue(Params[8].toInt());
                }
                if(Params[0].contains("Extruder") && Params.count() == 9)
                {
                    ui->seExtRefV->setValue(Params[1].toDouble());
                    ui->seExtR1Ohms->setValue(Params[2].toInt());
                    ui->seExtBeta->setValue(Params[3].toInt());
                    ui->seExtProbeOhms->setValue(Params[4].toInt());
                    ui->seExtProbeTemp->setValue(Params[5].toDouble());
                    ui->txtExtElementPin->setText(Params[6]);
                    ui->seExtADCCh->setValue(Params[7].toInt());
                    ui->seExtPWM->setValue(Params[8].toInt());
                }
            }
            else if(Line.contains("ADCConfig"))
            {
                QStringList Params = Line.split(";");
                if(Params[0].contains("ADC1") && Params.count() == 5)
                {
                    ui->txtClock->setText(Params[1]);
                    ui->txtMISO->setText(Params[2]);
                    ui->txtMOSI->setText(Params[3]);
                    ui->txtCS->setText(Params[4]);
                }
            }
        }
        ProbeCfg.close();
    }
    else
        QMessageBox::critical(this, "Error Opening File!", ProbeCfg.errorString(), QMessageBox::Ok);
}
