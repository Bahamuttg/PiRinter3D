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
#ifndef ADCCONTROLLER_H
#define ADCCONTROLLER_H

#include <QObject>
#include <QMutex>

#include "pigpio.h"

class ADCController : public QObject
{
private:
    Q_OBJECT

    int _Clock, _CS, _MISO, _MOSI;

    QMutex _Mutex;

public:
    explicit ADCController(const int &Clock, const int &MISO, const int &MOSI, const int &CS, QObject *parent = 0);
    ~ADCController();

    int GetChannelValue(const unsigned int &Channel);

    int GetChannelAverage(const unsigned int &Channel, const int &Reads);

signals:

public slots:

};

#endif // ADCCONTROLLER_H
