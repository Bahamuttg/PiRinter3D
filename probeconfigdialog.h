#ifndef PROBECONFIGDIALOG_H
#define PROBECONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class ProbeConfigDialog;
}

class ProbeConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProbeConfigDialog(QWidget *parent = 0);
    ~ProbeConfigDialog();

private:
    Ui::ProbeConfigDialog *ui;
};

#endif // PROBECONFIGDIALOG_H
