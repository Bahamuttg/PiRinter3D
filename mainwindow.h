#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QLabel>
#include <QProgressBar>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QThread>

#include "steppermotor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    QLabel *_StatusLabel;
    QProgressBar *_ProgressBar;
    QString _PrintFilePath ;

    void LoadConfigurations();

    void SaveConfigurations();

    void InitializeMotors();

public:
    StepperMotor *XAxis;
    StepperMotor *YAxis;
    StepperMotor *ZAxis;
    StepperMotor *ExtAxis;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_action_Stepper_Utility_triggered();

    void on_action_Exit_triggered();

    void on_action_Load_3D_Print_triggered();

    void on_action_Configure_PiRinter_triggered();


    void UpdateMotorConfig();

    void UpdateTempConfig();

};

#endif // MAINWINDOW_H
