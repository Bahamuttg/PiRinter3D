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
    BedProbeWorker = 0;
    ExtProbeWorker = 0;

    InitializeThermalProbes();
}

HeaterDriver::~HeaterDriver()
{
    delete ui;
    delete _ExtProbe;
    delete _BedProbe;
    BedProbeWorker->Terminate();
    ExtProbeWorker->Terminate();
    ExtProbeWorker->wait();
    BedProbeWorker->wait();
    delete BedProbeWorker;
    delete ExtProbeWorker;
    delete _ADCController;
}

void HeaterDriver::on_cbEnable_toggled(bool checked)
{
    //If the rb bed is checked disconnect the ext and connect the bed
    //Vise versa for the ext if it's checked.

    //Connect the probe reads to the lcd displays
    connect(this->BedProbeWorker, SIGNAL(ReportTemp(int)), ui->lcdTemp, SLOT(display(int)));
    connect(this->ExtProbeWorker, SIGNAL(ReportTemp(int)), ui->lcdTemp, SLOT(display(int)));
    //connect the Probe element states to the UI
    connect(this->BedProbeWorker, SIGNAL(ReportElementState(bool)), ui->chkOn, SLOT(setChecked(bool)));
    connect(this->ExtProbeWorker, SIGNAL(ReportElementState(bool)), ui->chkOn, SLOT(setChecked(bool)));
    //Connect the Temp Setter
    connect(ui->seTemp, SIGNAL(valueChanged(int)), this, SLOT(ChangeBedTemp(int)));
    connect(ui->seTemp, SIGNAL(valueChanged(int)), this, SLOT(ChangeExtTemp(int)));
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
                {
                    _ExtProbe = new ThermalProbe(Params[1].toDouble(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(),
                            Params[5].toDouble(), Params[6].toInt(),  Params[7].toInt(), _ADCController);
                    ExtProbeWorker = new ProbeWorker(_ExtProbe, Params[8].toInt(), this);
                    connect(this, SIGNAL(terminated()), ExtProbeWorker, SLOT(terminate()));
                }
                if(Params[0].contains("Bed"))
                {
                    _BedProbe = new ThermalProbe(Params[1].toDouble(), Params[2].toInt(), Params[3].toInt(), Params[4].toInt(),
                            Params[5].toDouble(), Params[6].toInt(),  Params[7].toInt(), _ADCController);
                    BedProbeWorker = new ProbeWorker(_BedProbe, Params[8].toInt(), this);
                    connect(this, SIGNAL(terminated()), BedProbeWorker, SLOT(terminate()));
                }
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

void HeaterDriver::ChangeBedTemp(const int &Celsius)
{
    this->BedProbeWorker->SetTargetTemp(Celsius);
}

void HeaterDriver::ChangeExtTemp(const int &Celsius)
{
    this->ExtProbeWorker->SetTargetTemp(Celsius);
}
