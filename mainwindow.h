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
#include <QMessageBox>

#include "gcodeinterpreter.h"
#include "probeconfigdialog.h"
#include "stepperdriver.h"
#include "motorconfigdialog.h"
#include "motorworker.h"

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

    QTimer *ButtonTimer;

    GCodeInterpreter *_Interpreter;

    StepperMotor *_XAxis, *_YAxis, *_ZAxis, *_ExtAxis;
    MotorWorker *_XWorker, *_YWorker, *_ZWorker, *_ExtWorker;
    QThread *_XMotorThread, *_YMotorThread, *_ZMotorThread, *_ExtMotorThread;


    void LoadConfigurations();

    void SaveConfigurations();

    void InitializeMotors();
public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:

    void on_BedTempOverride(bool Arg);
    void on_ExtTempOverride(bool Arg);
    void on_action_Stepper_Utility_triggered();
    void on_action_Exit_triggered();
    void on_action_Load_3D_Print_triggered();
    void on_actionS_tart_triggered();
    void on_action_Stop_triggered();
    void on_action_Configure_Motors_triggered();
    void on_actionSetup_Print_Area_triggered();
    void on_actionConfigure_Probes_triggered();
    void on_actionPre_Heat_Elements_triggered();

    void UpdatePositionLabel(QString Value);
    void UpdateMotorSettings();
    void DisplayError(QString Msg, QString Title = "An Error Occoured!")
    {
         QMessageBox::critical(0, Title, Msg, QMessageBox::Ok);
    }
};

#endif // MAINWINDOW_H
