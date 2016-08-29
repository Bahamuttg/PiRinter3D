#ifndef PRINTAREACONFIGDIALOG_H
#define PRINTAREACONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class PrintAreaConfigDialog;
}

class PrintAreaConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintAreaConfigDialog(QWidget *parent = 0);
    ~PrintAreaConfigDialog();

private:
    Ui::PrintAreaConfigDialog *ui;
};

#endif // PRINTAREACONFIGDIALOG_H
