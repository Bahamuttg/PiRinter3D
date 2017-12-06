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
#include "mainwindow.h"
#include <QApplication>
#include <pigpio.h>

int main(int argc, char *argv[])
{
	if (gpioInitialise() < 0) 
		return 1;
	//TODO: Pull low these GPIO pins on startup!

    // SDA0 GPIO 0
    gpioSetMode(17, PI_OUTPUT);
    gpioWrite(17, PI_LOW);
    // SCL0 GPIO 1
    gpioSetMode(18, PI_OUTPUT);
    gpioWrite(18, PI_LOW);
    // SDA1 GPIO 2
    gpioSetMode(27, PI_OUTPUT);
    gpioWrite(27, PI_LOW);
    // SCL1 GPIO 3
    gpioSetMode(22, PI_OUTPUT);
    gpioWrite(22, PI_LOW);
    // GPIO 4
    gpioSetMode(23, PI_OUTPUT);
    gpioWrite(23, PI_LOW);
    // GPIO 5
    gpioSetMode(24, PI_OUTPUT);
    gpioWrite(24, PI_LOW);
    //  GPIO 6
    gpioSetMode(25, PI_OUTPUT);
    gpioWrite(25, PI_LOW);
    //  GPIO 7
    gpioSetMode(4, PI_OUTPUT);
    gpioWrite(4, PI_LOW);
    //  GPIO 8
    gpioSetMode(2, PI_OUTPUT);
    gpioWrite(2, PI_LOW);
    //  GPIO 9
    gpioSetMode(3, PI_OUTPUT);
    gpioWrite(3, PI_LOW);
    //    //  GPIO 34
    //    gpioSetMode(4, PI_OUTPUT);
    //    gpioWrite(4, PI_LOW);
    //    //  GPIO 35
    //    gpioSetMode(4, PI_OUTPUT);
    //    gpioWrite(4, PI_LOW);
    //    //  GPIO 36
    //    gpioSetMode(4, PI_OUTPUT);
    //    gpioWrite(4, PI_LOW);

    QApplication a(argc, argv);
    MainWindow w;

    w.show();
    return a.exec();
}
