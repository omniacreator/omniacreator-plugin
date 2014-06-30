/***************************************************************************//**
* @file
* Demo Port
*
* @version @n 1.0
* @date @n 4/5/2014
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2014 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 4/5/2014
*******************************************************************************/

#include "demoport.h"

DemoPort::DemoPort(QObject *parent) : SerialDevice(parent)
{
    core = new ILCore;
    oscilloscope = new ILOscilloscope;
    graph0 = new ILOscilloscopeGraph;
    graph1 = new ILOscilloscopeGraph;
}

DemoPort::~DemoPort()
{
    delete graph1;
    delete graph0;
    delete oscilloscope;
    delete core;
}

void DemoPort::setup()
{
    core->init();
    core->clearAll();

    oscilloscope->initDocked(core, "Sin & Cos Waves");
    oscilloscope->setBackgroundColor(0xFFFFFF);
    oscilloscope->setXAxisLabel("x");
    oscilloscope->setYAxisLabel("y");

    graph0->init(oscilloscope, "Sin");
    graph0->setFillColor(0x7FC8C8FF);
    graph0->setFillStyle(FS_SOLID_PATTERN);

    graph1->init(oscilloscope, "Cos");
    graph1->setFillColor(0x7FFFC8C8);
    graph1->setFillStyle(FS_SOLID_PATTERN);
}

void DemoPort::loop()
{
    static float degree = 0;

    graph0->addKeyValueF(degree, sin(qDegreesToRadians(degree)));
    graph1->addKeyValueF(degree, cos(qDegreesToRadians(degree)));

    degree += 1;
}

/***************************************************************************//**
* @file
* @par MIT License - TERMS OF USE:
* @n Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* @n
* @n The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* @n
* @n THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/
