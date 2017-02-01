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

    void on_chkExtCtrlX_toggled(bool checked);

    void on_chkExtCtrlY_toggled(bool checked);

    void on_chkExtCtrlZ_toggled(bool checked);

    void on_chkExtCtrlEX_toggled(bool checked);

signals:
    void OnConfigChanged();
};

#endif // MOTORCONFIGDIALOG_H
