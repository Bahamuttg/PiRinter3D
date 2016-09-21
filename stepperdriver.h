#ifndef StepperDriver_H
#define StepperDriver_H

#include <QMainWindow>
#include "steppermotor.h"
#include "motorworker.h"
#include "ui_stepperdriver.h"
#include <QtCore>
#include <QPushButton>
#include <QTimer>
#include <QThread>
#include <QMessageBox>

namespace Ui {
class StepperDriver;
}

class StepperDriver : public QMainWindow
{
private:
    Ui::StepperDriver *ui;

    Q_OBJECT
    bool StopThread;

    void UpdateLabels();

public:
    StepperMotor *Motor;
    QTimer *ButtonTimer;
    QThread *MotorThread;
    MotorWorker *Worker;
    explicit StepperDriver(QWidget *parent = 0);
    ~StepperDriver();

private slots:
    void on_pushButton_2_pressed();
    void on_pushButton_pressed();

    void on_pushButton_released();
    void on_pushButton_2_released();

    void errorString(QString);

    void UpdatePositionLabel(QString);

    void UpdateMotorSettings();

    void InitializeMotor(const QString &MotorName);

signals:
    void CoilSettingsChanged();

};

#endif // StepperDriver_H
