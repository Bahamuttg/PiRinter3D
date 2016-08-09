#ifndef STEPPERMOTOR_H
#define STEPPERMOTOR_H

#include <wiringPi.h>
#include <string>

using namespace std;
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

    string MotorName;
    MotorDirection Direction;
    long Position;
    volatile bool HoldPosition;
    explicit StepperMotor(int Coil1, int Coil3, bool IsHalfStep = false, string Name = "Default");
    explicit StepperMotor(int Coil1, int Coil2, int Coil3, int Coil4, bool IsHalfStep = false, string Name = "Default");
    ~StepperMotor();

    void Rotate(MotorDirection Direction, const long &Steps, const int &MS_Delay);
    void Rotate(MotorDirection Direction, const int &MS_Delay);
    void StopRotation(const bool &Hold = true);
    void Enable();
    void Disable();
    void SetInverted(const bool &Arg = true);
    void SetNotGated(const bool &Arg = false);


protected:
    void PerformStep(MotorDirection);
};

#endif // STEPPERMOTOR_H
