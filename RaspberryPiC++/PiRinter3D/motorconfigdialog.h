#ifndef MOTORCONFIGDIALOG_H
#define MOTORCONFIGDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QObject>

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


private:
    Ui::MotorConfigDialog *ui;
};

#endif // MOTORCONFIGDIALOG_H
