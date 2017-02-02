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
        OutStream << "PrintAreaCfg::ZArea;" << ui->seLength->value() <<"\n";

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
                if(Params[0].contains("ZArea"))
                {
                    ui->seMaxHeight->setValue(Params[1].toInt());
                }
            }
        }
        AreaCfg.close();
    }
    else
        QMessageBox::critical(this, "Error Opening File!", AreaCfg.errorString(), QMessageBox::Ok);
}
