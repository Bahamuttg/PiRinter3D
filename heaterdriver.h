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

    ProbeWorker  *Worker;
    ~HeaterDriver();

    int GetTemp()
    {
        return this->Worker->TriggerProbeRead();
    }
    void SetTemp(const int &CelsiusValue)
    {
        this->Worker->SetTargetTemp(CelsiusValue);
    }

private slots:
    void ChangeTemp(const int &Celsius);
    void on_chkEnable_toggled(bool checked);
};

#endif // HEATERDRIVER_H
