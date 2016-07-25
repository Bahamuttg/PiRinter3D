#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stepperdriver.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
//==========================Main Menu Handlers=======================================
void MainWindow::on_action_Stepper_Utility_triggered()
{
    StepperDriver *SD = new StepperDriver(this);
    SD->show();

}

void MainWindow::on_action_Exit_triggered()
{
    this->close();
}
//==============End Main Menu Handlers================================================
