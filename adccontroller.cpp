#include "adccontroller.h"
#include "pigpio.h"
#include <QMutex>
ADCController::ADCController(const unsigned int &Clock, const unsigned int &MOSI, const unsigned int &MISO, const unsigned int &CS, QObject *parent) :
    QObject(parent)
{
    _Clock = Clock;
    _CS = CS;
    _MISO = MISO;
    _MOSI = MOSI;

    gpioSetMode(_Clock, PI_OUTPUT);
    gpioSetMode(_MOSI, PI_OUTPUT);
    gpioSetMode(_CS, PI_OUTPUT);
}

 ADCController::~ADCController()
 {
        gpioWaveClear();
        gpioWrite(_Clock, PI_LOW);
        gpioWrite(_MISO, PI_LOW);
        gpioWrite(_MOSI, PI_LOW);
 }

int ADCController::GetChannelValue(const unsigned int &Channel)
{
    rawSPI =
    {
       .clk     =  _Clock, // GPIO for SPI clock.
       .mosi    = _MOSI, // GPIO for SPI MOSI.
       .ss_pol  =  1, // Slave select resting level.
       .ss_us   =  1, // Wait 1 micro after asserting slave select.
       .clk_pol =  0, // Clock resting level.
       .clk_pha =  0, // 0 sample on first edge, 1 sample on second edge.
       .clk_us  =  1, // 2 clocks needed per bit so 500 kbps.
    };

    /*
    MCP3004/8 10-bit ADC 4/8 channels

    1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17
    SB SD D2 D1 D0 NA NA B9 B8 B7 B6 B5 B4 B3 B2 B1 B0

    SB       1
    SD       0=differential 1=single
    D2/D1/D0 0-7 channel
    */

    _Mutex.lock();
        /*
                           void getReading(
                           int adcs,  // Number of attached ADCs.
                           int *MISO, // The GPIO connected to the ADCs data out.
                           int OOL,   // Address of first OOL for this reading.
                           int bytes, // Bytes between readings.
                           int bits,  // Bits per reading.
                           char *buf)
                        {
                           int i, a, p;
                           uint32_t level;

                           p = OOL;

                           for (i=0; i<bits; i++)
                           {
                              level = rawWaveGetOut(p);

                              for (a=0; a<adcs; a++)
                              {
                                 putBitInBytes(i, buf+(bytes*a), level & (1<<MISO[a]));
                              }

                              p--;
                           }
                        }
         */
    _Mutex.unlock();
}
