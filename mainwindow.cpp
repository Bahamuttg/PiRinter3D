#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stepperdriver.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    _ProgressBar = new QProgressBar(this);
    _ProgressBar->setRange(0,100);
    _ProgressBar->reset();
    _StatusLabel = new QLabel(this);
    _StatusLabel->setText("Ready");
    ui->setupUi(this);

    ui->statusBar->insertPermanentWidget(0, _ProgressBar, 100);
    ui->statusBar->insertPermanentWidget(1, _StatusLabel, 250);
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
