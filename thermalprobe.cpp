#include "thermalprobe.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>

ThermalProbe::ThermalProbe(const unsigned int &Channel, const unsigned int &SPIPinBase, const unsigned int &TriggerPin, const int &TargetTemp)
{
    _PinBase = SPIPinBase;
    _Channel = Channel;
    _TriggerPin = TriggerPin;
    _TargetTemp = TargetTemp;

    pinMode(_TriggerPin, OUTPUT);

    mcp3004Setup(_PinBase, _Channel);
}

ThermalProbe::~ThermalProbe()
{
}

int ThermalProbe::MeasureTemp()
{

    /*
     * #function to read ADC channel value and calcuate tempurature of thermistor
    #Values:
    #   Vin = 3.3V
    #   Vout = 3.3V(ADC Value/1024)
    #   R1 = 1K Ohms
    #   R2 = thermRes = Estimated Thermistor Resistance
    #
    #   Beta = 3974 @ 0C/50C
    #
    #   25 C = 298.15 K
    def getTempAtADCChannel(channel):
        adcVal = mcp.read_adc(channel);
        Vout = 3.3 * (float(adcVal)/1024);
        thermRes = (Vout * 1000)/(3.3 - Vout);
        return ((ThermDefaultTempK * ThermBeta) / log(ThermDefaultRes/thermRes) / (ThermBeta /  log(ThermDefaultRes/thermRes) - ThermDefaultTempK) - 273.15);
     */
}

void ThermalProbe::TriggerElement(ElementState Value)
{
    if(Value == ThermalProbe::OFF)
        digitalWrite(_TriggerPin, LOW);
    else
        digitalWrite(_TriggerPin, HIGH);
    ElementCurrentState = Value;
}

void ThermalProbe::SetTargetTemp(const int &Value )
{
    _TargetTemp = Value;
    MeasureTemp();
}
