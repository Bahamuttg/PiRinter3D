#ifndef MOTORSETUP_H
#define MOTORSETUP_H

#include <QTabWidget>

namespace Ui {
class MotorSetup;
}

class MotorSetup : public QTabWidget
{
    Q_OBJECT

public:
    explicit MotorSetup(QWidget *parent = 0);
    ~MotorSetup();

private:
    Ui::MotorSetup *ui;
};

#endif // MOTORSETUP_H
