#ifndef HEATERDRIVER_H
#define HEATERDRIVER_H
#include <QMainWindow>
#include <QtCore>
#include <QPushButton>
#include <QTimer>
#include <QThread>
#include <QMessageBox>
#include "thermalprobe.h"
#include "probeworker.h"
#include "adccontroller.h"

namespace Ui {
class HeaterDriver;
}

class HeaterDriver : public QMainWindow
{
private:
    Q_OBJECT
    Ui::HeaterDriver *ui;

    ThermalProbe *_BedProbe, *_ExtProbe;
    //Need a pointer so we can share this resource with the probes.
    ADCController *_ADCController;

    void InitializeThermalProbes();
    void InitializeADCConverter();

public:
    explicit HeaterDriver(QWidget *parent = 0);

    ProbeWorker *BedProbeWorker, *ExtProbeWorker;
    ~HeaterDriver();

    int GetExtruderTemp()
    {
        return this->ExtProbeWorker->TriggerProbeRead();
    }
    void SetExtruderTemp(const int &CelsiusValue)
    {
        this->ExtProbeWorker->SetTargetTemp(CelsiusValue);
    }

    int GetBedTemp()
    {
        return this->BedProbeWorker->TriggerProbeRead();
    }
    void SetBedTemp(const int &CelsiusValue)
    {
        this->BedProbeWorker->SetTargetTemp(CelsiusValue);
    }

private slots:
    void on_cbEnable_toggled(bool checked);

    void ChangeBedTemp(const int &Celsius);

    void ChangeExtTemp(const int &Celsius);
};

#endif // HEATERDRIVER_H
