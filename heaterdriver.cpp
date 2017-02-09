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
}

HeaterDriver::~HeaterDriver()
{
    delete ui;
}
