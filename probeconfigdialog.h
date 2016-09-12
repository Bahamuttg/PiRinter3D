#ifndef PROBECONFIGDIALOG_H
#define PROBECONFIGDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QObject>
#include <QFile>
#include <QTextStream>

namespace Ui {
class ProbeConfigDialog;
}

class ProbeConfigDialog : public QDialog
{
    Q_OBJECT

    void WriteCfgFile();

    void ReadCfgFile();

    Ui::ProbeConfigDialog *ui;

public:
    explicit ProbeConfigDialog(QWidget *parent = 0);
    ~ProbeConfigDialog();

private slots:

    void on_buttonBox_accepted();

signals:
    void OnConfigChanged();
};

#endif // PROBECONFIGDIALOG_H
