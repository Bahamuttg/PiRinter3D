#ifndef PRINTAREACONFIGDIALOG_H
#define PRINTAREACONFIGDIALOG_H

#include <QDialog>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

namespace Ui {
class PrintAreaConfigDialog;
}

class PrintAreaConfigDialog : public QDialog
{
    Q_OBJECT
private:

    void WriteCfgFile();

    void ReadCfgFile();

    Ui::PrintAreaConfigDialog *ui;

public:
    explicit PrintAreaConfigDialog(QWidget *parent = 0);
    ~PrintAreaConfigDialog();

private slots:
    void on_buttonBox_accepted();

signals:
    void OnConfigChanged();

};

#endif // PRINTAREACONFIGDIALOG_H
