#include "printareaconfigdialog.h"
#include "ui_printareaconfigdialog.h"



PrintAreaConfigDialog::PrintAreaConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintAreaConfigDialog)
{
    ui->setupUi(this);
    ReadCfgFile();
}

PrintAreaConfigDialog::~PrintAreaConfigDialog()
{
    delete ui;
}

void PrintAreaConfigDialog::on_buttonBox_accepted()
{
    this->WriteCfgFile();
    emit OnConfigChanged();
}

void PrintAreaConfigDialog::WriteCfgFile()
{
    QFile AreaCfg("AreaCfg.ini");
    if(AreaCfg.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QTextStream OutStream(&AreaCfg);
        OutStream << "PrintAreaCfg::XArea;" << ui->seWidth->value() <<"\n";
        OutStream << "PrintAreaCfg::YArea;" << ui->seLength->value() <<"\n";
        OutStream << "EndStopCfg::XStop;" << ui->txtXStop->text() <<"\n";
        OutStream << "EndStopCfg::YStop;" << ui->txtYStop->text() <<"\n";
        OutStream << "EndStopCfg::ZStop;" << ui->txtZStop->text() <<"\n";

        OutStream.flush();
        AreaCfg.close();
    }
    else
        QMessageBox::critical(this, "Error Opening File!", AreaCfg.errorString(), QMessageBox::Ok);
}

void PrintAreaConfigDialog::ReadCfgFile()
{
    QFile AreaCfg("AreaCfg.ini");
    if(AreaCfg.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        QTextStream InStream(&AreaCfg);
        while(!InStream.atEnd())
        {
            QString Line = InStream.readLine(); //read one line at a time
            if(Line.contains("PrintAreaCfg"))
            {
                QStringList Params = Line.split(";");
                if(Params[0].contains("XArea"))
                {
                    ui->seWidth->setValue(Params[1].toInt());
                }
                if(Params[0].contains("YArea"))
                {
                    ui->seLength->setValue(Params[1].toInt());
                }
            }
            else if(Line.contains("EndStopCfg"))
            {
                QStringList Params = Line.split(";");
                if(Params[0].contains("XStop"))
                {
                    ui->txtXStop->setText(Params[1]);
                }
                if(Params[0].contains("YStop"))
                {
                    ui->txtXStop->setText(Params[1]);
                }
                if(Params[0].contains("ZStop"))
                {
                    ui->txtXStop->setText(Params[1]);
                }
            }
        }
        AreaCfg.close();
    }
    else
        QMessageBox::critical(this, "Error Opening File!", AreaCfg.errorString(), QMessageBox::Ok);
}
