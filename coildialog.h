#ifndef COILDIALOG_H
#define COILDIALOG_H

#include <QDialog>

namespace Ui {
class CoilDialog;
}

class CoilDialog : public QDialog
{
    Q_OBJECT

public:
    int A1;
    int A2;
    int B1;
    int B2;

    explicit CoilDialog(int A1, int A2, int B1, int B2, QWidget *parent = 0);
    ~CoilDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void Value_ChangedA1(const QString &);
    void Value_ChangedA2(const QString &);
    void Value_ChangedB1(const QString &);
    void Value_ChangedB2(const QString &);

private:
    Ui::CoilDialog *ui;
};

#endif // COILDIALOG_H
