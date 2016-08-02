#ifndef STEPPERMOTOR_H
#define STEPPERMOTOR_H

#include <wiringPi.h>

namespace Stepper
{
    class StepperMotor;
}
class StepperMotor
{
private:
    unsigned int _Coil_1;
    unsigned int _Coil_2;
    unsigned int _Coil_3;
    unsigned int _Coil_4;
    unsigned int _Phase;

	volatile bool _IsNOTGated;
    volatile bool _IsHalfStep;
    volatile bool _Enabled;
    volatile bool _IsInverted;
    volatile bool _IsRotating;

    void CoilsOff();
    void InvertDirection();

public:
    enum MotorDirection
    {
        CLOCKWISE = 1,
        CTRCLOCKWISE = -1
    };

    MotorDirection Direction;
    long Position;
    volatile bool HoldPosition;
    explicit StepperMotor(int Coil1, int Coil3, bool IsHalfStep = false);
    explicit StepperMotor(int Coil1, int Coil2, int Coil3, int Coil4, bool IsHalfStep = false);
    ~StepperMotor();

    void Rotate(MotorDirection Direction, long Steps, int MS_Delay);
    void Rotate(MotorDirection Direction, int MS_Delay);
    void StopRotation(const bool &Hold = true);
    void Enable();
    void Disable();
    void SetInverted(const bool &Arg = true);


protected:
    void PerformStep(MotorDirection);
};

#endif // STEPPERMOTOR_H
