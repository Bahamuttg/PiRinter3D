#ifndef HEATERDRIVER_H
#define HEATERDRIVER_H
#include <QMainWindow>
#include <QtCore>
#include <QPushButton>
#include <QTimer>
#include <QThread>
#include <QMessageBox>

namespace Ui {
class HeaterDriver;
}

class HeaterDriver : public QMainWindow
{
    Q_OBJECT

public:
    explicit HeaterDriver(QWidget *parent = 0);
    ~HeaterDriver();

private:
    Ui::HeaterDriver *ui;
};

#endif // HEATERDRIVER_H
