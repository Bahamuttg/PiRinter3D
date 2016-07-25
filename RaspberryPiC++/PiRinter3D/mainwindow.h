#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_Stepper_Utility_triggered();

    void on_action_Exit_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
