#include "thermalprobe.h"

ThermalProbe::ThermalProbe()
{
}
/*
[‎8/‎16/‎2016 1:38 PM] Jason Johnson: 
def getTempAtADCChannel(channel):
    adcVal = mcp.read_adc(channel);
    Vout = 3.3 * float(adcVal/1024);
    thermRes = (Vout * 1000)/(3.3 - Vout);
    return (ThermDefaultTempK * ThermBeta) / Log(ThermDefaultRes/thermRes) / (ThermBeta /  Log(ThermDefaultRes/thermRes) - ThermDefaultTempK); 

    TODO:: need global default
*/
