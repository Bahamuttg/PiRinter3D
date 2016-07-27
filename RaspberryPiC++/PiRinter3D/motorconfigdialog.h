#ifndef MOTORCONFIGDIALOG_H
#define MOTORCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class MotorConfigDialog;
}

class MotorConfigDialog : public QDialog
{
    Q_OBJECT

public:

    explicit MotorConfigDialog(QWidget *parent = 0);
    ~MotorConfigDialog();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::MotorConfigDialog *ui;
};

#endif // MOTORCONFIGDIALOG_H
