#ifndef THERMALPROBE_H
#define THERMALPROBE_H
#include <wiringPiSPI.h>
#include <mcp3004.h>
class ThermalProbe
{
private:
    unsigned int  _Channel, _PinBase;

public:
    ThermalProbe(const unsigned int &Channel, const unsigned int &PinBase);
    ~ThermalProbe();

    int MeasureTemp();
};

#endif // THERMALPROBE_H
