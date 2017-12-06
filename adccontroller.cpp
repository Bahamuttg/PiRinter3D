/*
* =================BEGIN GPL LICENSE BLOCK=========================================
*  Author: Thomas George
*  PiRinter3D Copyright (C) 2016
*
*  This program is free software; you can redistribute it and/or 
*  modify it under the terms of the GNU General Public License 
*  as published by the Free Software Foundation; either version 3 
*  of the License, or (at your option) any later version. 
* 
*  This program is distributed in the hope that it will be useful, 
*  but WITHOUT ANY WARRANTY; without even the implied warranty of 
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
*  GNU General Public License for more details. 
* 
*  You should have received a copy of the GNU General Public License 
*  along with this program; if not, write to the Free Software Foundation, 
*  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. 
* 
* =================END GPL LICENSE BLOCK=========================================
*/
#include "adccontroller.h"
#include "pigpio.h"
#include <QMutex>
#include <QDebug>

ADCController::ADCController(const int &Clock, const int &MISO, const int &MOSI, const int &CS, QObject *parent) :
    QObject(parent)
{
    _Clock = Clock;
    _CS = CS;
    _MISO = MISO;
    _MOSI = MOSI;

    gpioSetMode(_Clock, PI_OUTPUT);
    gpioSetMode(_MOSI, PI_OUTPUT);
    gpioSetMode(_MISO, PI_INPUT);
    gpioSetMode(_CS, PI_OUTPUT);

    gpioWrite(_Clock, PI_LOW);
    gpioWrite(_MOSI, PI_LOW);
    gpioWrite(_CS, PI_LOW);
}

ADCController::~ADCController()
{
    gpioWaveClear();
    gpioWrite(_Clock, PI_LOW);
    gpioWrite(_MISO, PI_LOW);
    gpioWrite(_MOSI, PI_LOW);
    gpioWrite(_CS, PI_LOW);
}

int ADCController::GetChannelAverage(const unsigned int &Channel, const int &Reads)
{
    if(Reads <= 0)
        return -1;
    int average =0;
    for(int i = 0; i < Reads; i++)
        average += GetChannelValue(Channel);
    //TODO: Throw out values too far away from the mean values.
    return average / Reads;
}

int ADCController::GetChannelValue(const unsigned int &Channel)
{	
    unsigned int RX = 0, TX = 0;
    if(!_Mutex.tryLock(300) || Channel > 7)//we must have exclusive access.
        return -1;

   /*
   MCP3004/8 10-bit ADC 4/8 channels

   1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17
   SB SD D2 D1 D0 NA NA B9 B8 B7 B6, B5 B4 B3 B2 B1 B0

   SB       1
   SD       0=differential 1=single
   D2/D1/D0 0-7 channel
   */

   //Now we need to set up 5 bits of data to push to the ADC.
   //So we need to take the channel number (three bits) and do a bitwise OR on it with 11000;
   //This gives us a leading 1 start bit and a 1 for the single-ended read. Then the three following bits
   //are the channel selection on the ADC.
   TX = Channel; //0000 0XXX
   TX |= 0x18; //Start bit + single/diff bit. 0001 1XXX
   //We only need to send 5 bits so we are going to left shift the bits 3 spaces to the front of the byte.
   TX <<= 3; //11XX X000

    gpioWrite(_CS, PI_HIGH);//Start with CS high so we can latch the clock.
    gpioWrite(_Clock, PI_LOW);//start clock on the low side.
    gpioWrite(_CS, PI_LOW);//Bring CS low now to start bit banging the ADC.

    //Now we need to send those bits to the ADC.
    for(int i = 0; i < 5; i++)
    {
        //TX starts with a 1? Do a bitwise AND to see if it matches the mask.
        if(TX & 0x80)// 0x80 is 1000 0000
            gpioWrite(_MOSI, PI_HIGH);
        else
            gpioWrite(_MOSI, PI_LOW);
        //now shift the bits in the TX byte left 1;
        TX <<= 1;
        //bounce the clock and start again...
        gpioWrite(_Clock, PI_HIGH);
        gpioDelay(200);
        gpioWrite(_Clock, PI_LOW);
    }

    //Now we need to read in one empty bit, one null bit, and the 10 bits with our value.
    for(int i = 0; i < 12; i++)
    {
        gpioWrite(_Clock, PI_HIGH);
        gpioDelay(200);
        gpioWrite(_Clock, PI_LOW);
        RX <<= 1; //Shift our bit over so when we read the MISO we can OR the value with RX.
        if(gpioRead(_MISO))
            RX |= 0x1;
    }

    gpioWrite(_CS, PI_HIGH); //Bring the CS pin back high to end our transaction.

    _Mutex.unlock();

    //right shift the first bit because it's the null one. What's left will be our value;
    RX >>= 1;
    return RX;
}
