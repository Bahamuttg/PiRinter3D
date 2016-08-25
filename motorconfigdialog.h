#ifndef MOTORCONFIGDIALOG_H
#define MOTORCONFIGDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QObject>
#include <QFile>
#include <QTextStream>

namespace Ui {
class MotorConfigDialog;
}

class MotorConfigDialog : public QDialog
{
private:
    Q_OBJECT

    void WriteCfgFile();

    void ReadCfgFile();

     Ui::MotorConfigDialog *ui;
public:

    explicit MotorConfigDialog(QWidget *parent = 0);
    ~MotorConfigDialog();

private slots:

    void on_buttonBox_accepted();

signals:
    void OnConfigChanged();
};

#endif // MOTORCONFIGDIALOG_H