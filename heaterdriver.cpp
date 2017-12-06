#include "heaterdriver.h"
#include "ui_heaterdriver.h"
#include <QMainWindow>
#include <QtCore>
#include <QTimer>
#include <QThread>
#include <QDebug>

HeaterDriver::HeaterDriver(QWidget *parent) :
      QMainWindow(parent),
    ui(new Ui::HeaterDriver)
{
    ui->setupUi(this);

    _ExtProbe = 0;
    _BedProbe = 0;
   Worker = 0;
    ui->lblOn->setVisible(false);
    InitializeThermalProbes();
}

HeaterDriver::~HeaterDriver()
{
    delete ui;
    delete _ExtProbe;
    delete _BedProbe;
    Worker->Stop();
    Worker->wait();
    delete Worker;
    delete _ADCController;
}

void HeaterDriver::on_chkEnable_toggled(bool checked)
{
    if(checked)
    {
        if(Worker == 0)
        {
            if(ui->rbBedElement->isChecked())
                Worker = new ProbeWorker(_BedProbe, 100);
            else if (ui->rbExtruderElement->isChecked())
                Worker = new ProbeWorker(_ExtProbe, 100);
            else
                return;

            //Connect the probe reads to the lcd displays
            connect(this->Worker, SIGNAL(ReportTemp(int)), ui->lcdTemp, SLOT(display(int)));

            //connect the Probe element states to the UI
            connect(this->Worker, SIGNAL(ReportElementState(bool)), ui->lblOn, SLOT(setVisible(bool)));

            //Connect the Temp Setter
            connect(ui->seTemp, SIGNAL(valueChanged(int)), this, SLOT(ChangeTemp(int)));

            Worker->SetTargetTemp(ui->seTemp->value());
            Worker->start();
        }
        else
            return;
    }
    else
    {
        if(Worker != 0)
        {
            Worker->Stop();
            Worker->wait();
            delete Worker;
            Worker = 0;
        }
    }
}

void HeaterDriver::InitializeThermalProbes()
{
    InitializeADCConverter();

    if(_ADCController == 0)
        return;

    QFile ProbeConfig("ProbeCfg.ini");
    if(ProbeConfig.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        QTextStream CfgStream(&ProbeConfig); //load config file
        while (!CfgStream.atEnd())
        {
            QString Line = CfgStream.readLine(); //read one line at a time
            if(Line.contains("ProbeCfg"))
            {
                QStringList Params = Line.split(";");
                if(Params[0].contains("Extruder"))
                    _ExtProbe = new ThermalProbe(Params[1].toDouble(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(),
                            Params[5].toDouble(), Params[6].toInt(),  Params[7].toInt(), _ADCController);

                if(Params[0].contains("Bed"))
                    _BedProbe = new ThermalProbe(Params[1].toDouble(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(),
                            Params[5].toDouble(), Params[6].toInt(),  Params[7].toInt(), _ADCController);
            }
        }
        ProbeConfig.close();
    }
    else
        QMessageBox::critical(0, "Error Opening File!", ProbeConfig.errorString(), QMessageBox::Ok);
}

void HeaterDriver::InitializeADCConverter()
{
    QFile ProbeConfig("ProbeCfg.ini");
    if(ProbeConfig.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        QTextStream CfgStream(&ProbeConfig); //load config file
        while (!CfgStream.atEnd())
        {
            QString Line = CfgStream.readLine(); //read one line at a time
            if(Line.contains("ADCConfig"))
            {
                QStringList Params = Line.split(";");
                if(Params[0].contains("ADC1"))
                {
                    _ADCController = new ADCController(Params[1].toInt(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(), this);
                }
            }
        }
        ProbeConfig.close();
    }
    else
        _ADCController = 0;
}

void HeaterDriver::ChangeTemp(const int &Celsius)
{
    if(Worker != 0)
        this->Worker->SetTargetTemp(Celsius);
}
