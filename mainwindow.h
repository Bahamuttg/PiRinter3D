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

    QTimer *_XButtonTimer, *_YButtonTimer, *_ZButtonTimer, *_ExtButtonTimer;

    GCodeInterpreter *_Interpreter;

    StepperMotor *_XAxis, *_YAxis, *_ZAxis, *_ExtAxis;
    MotorWorker *_XWorker, *_YWorker, *_ZWorker, *_ExtWorker;
    QThread *_XMotorThread, *_YMotorThread, *_ZMotorThread, *_ExtMotorThread;


    void LoadConfigurations();
    void SaveConfigurations();
    void InitializeMotors();
    void InitializeMotorWorkers();
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

    void UpdatePositionLabel(QString Name, const long Pos);
    void UpdateMotorSettings();
    void DisplayError(QString Msg, QString Title = "An Error Occoured!")
    {
         QMessageBox::critical(0, Title, Msg, QMessageBox::Ok);
    }
    void on_btnXLeft_pressed();
    void on_btnXRight_released();
    void on_btnYBack_pressed();
    void on_btnXLeft_released();
    void on_btnXRight_pressed();
    void on_btnYBack_released();
    void on_btnYFore_pressed();
    void on_btnYFore_released();
    void on_btnZUp_pressed();
    void on_btnZUp_released();
    void on_btnZDown_pressed();
    void on_btnZDown_released();
    void on_btnEXTFore_pressed();
    void on_btnEXTFore_released();
    void on_btnEXTBack_pressed();
    void on_btnEXTBack_released();
};

#endif // MAINWINDOW_H
