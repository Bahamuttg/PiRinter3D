#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H
#include <QtCore>
#include "steppermotor.h"

class MotorController : public QObject
{
	Q_OBJECT
public:
    StepperMotor::MotorDirection GetDirection(const int &Step);
    StepperMotor::MotorDirection GetReverseDirection(const int &Step);

    void StepMotor(StepperMotor &Motor, long Steps, const int &MSDelay);
    void StepMotors(StepperMotor &Motor1, long Steps1, StepperMotor &Motor2, long Steps2, const float &MSDelay);
    void StepMotors(StepperMotor &Motor1, long Steps1, StepperMotor &Motor2, long Steps2,
                    StepperMotor &Motor3, long Steps3, const float &MSDelay);
    void StepMotors(StepperMotor &Motor1, long Steps1, StepperMotor &Motor2, long Steps2,
                    StepperMotor &Motor3, long Steps3,  StepperMotor &Motor4, long Steps4, const float &MSDelay);
};

#endif // MOTORCONTROLLER_H
