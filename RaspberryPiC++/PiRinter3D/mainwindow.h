#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QLabel>
#include <QProgressBar>
#include "steppermotor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QLabel *_StatusLabel;
    QProgressBar *_ProgressBar;

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

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
