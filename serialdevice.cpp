/***************************************************************************//**
* @file
* Serial Device
*
* @version @n 1.0
* @date @n 4/4/2014
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2014 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 4/4/2014
*******************************************************************************/

#include "serialdevice.h"

QByteArray SerialDevice::m_readBuffer = QByteArray();
QByteArray SerialDevice::m_writeBuffer = QByteArray();

SerialDevice *SerialDevice::m_port = NULL;

const char *SerialDevice::m_errorStrings[] =
{
    QT_TR_NOOP("Closed Error"),
    QT_TR_NOOP("Opened Error")
};

SerialDevice::SerialDevice(QObject *parent) : QIODevice(parent)
{
    connect(this, SIGNAL(aboutToClose()), this, SIGNAL(readChannelFinished()));
}

SerialDevice::~SerialDevice()
{
    close();
}

bool SerialDevice::open(OpenMode mode)
{
    bool result = (!m_port) && QIODevice::open(mode);

    if(result)
    {
        m_port = this; QTimer::singleShot(0, this, SLOT(setupRunner()));
    }
    else
    {
        setErrorString(m_errorStrings[1]);
    }

    return result;
}

void SerialDevice::close()
{
    if(isOpen())
    {
        QIODevice::close(); clearInput(); clearOutput(); m_port = NULL;
    }
}

void SerialDevice::clearInput()
{
    if(m_port)
    {
        m_readBuffer.clear();
    }
}

void SerialDevice::clearOutput()
{
    if(m_port)
    {
        m_writeBuffer.clear();
    }
}

bool SerialDevice::atEnd() const
{
    if(!isOpen())
    {
        return false;
    }

    return !bytesAvailable();
}

bool SerialDevice::canReadLine() const
{
    if(!isOpen())
    {
        return false;
    }

    return m_readBuffer.contains('\n') || QIODevice::canReadLine();
}

bool SerialDevice::isSequential() const
{
    if(!isOpen())
    {
        return false;
    }

    return true;
}

qint64 SerialDevice::bytesAvailable() const
{
    if(!isOpen())
    {
        return 0;
    }

    return m_readBuffer.size() + QIODevice::bytesAvailable();
}

qint64 SerialDevice::bytesToWrite() const
{
    return 0;
}

bool SerialDevice::atEndExtern()
{
    if(!m_port)
    {
        return false;
    }

    return !bytesAvailableExtern();
}

bool SerialDevice::canReadLineExtern()
{
    if(!m_port)
    {
        return false;
    }

    return m_writeBuffer.contains('\n');
}

bool SerialDevice::isSequentialExtern()
{
    if(!m_port)
    {
        return false;
    }

    return true;
}

qint64 SerialDevice::bytesAvailableExtern()
{
    if(!m_port)
    {
        return 0;
    }

    return m_writeBuffer.size();
}

qint64 SerialDevice::bytesToWriteExtern()
{
    return 0;
}

qint64 SerialDevice::millisecondsExtern()
{
    return QDateTime::currentMSecsSinceEpoch();
}

void SerialDevice::waitMillisecondsExtern(qint64 milliseconds)
{
    QThread::msleep(milliseconds);
}

void SerialDevice::resetEventExtern()
{
    if(m_port)
    {
        emit m_port->resetEvent();
    }
}

qint64 SerialDevice::readDataExtern(char *data)
{
    return readDataExtern(data, sizeof(char));
}

qint64 SerialDevice::writeDataExtern(char *data)
{
    return writeDataExtern(data, sizeof(char));
}

qint64 SerialDevice::readDataExtern(char *data, qint64 maxSize)
{
    if(!m_port)
    {
        return 0;
    }

    qint64 size = qMin(m_writeBuffer.size(), int(maxSize));
    qMemCopy(data, m_writeBuffer.constData(), size);
    m_writeBuffer.remove(0, size); return size;
}

qint64 SerialDevice::writeDataExtern(const char *data, qint64 maxSize)
{
    if(!m_port)
    {
        return 0;
    }

    m_readBuffer.append(data, maxSize);
    emit m_port->readyRead();
    return maxSize;
}

qint64 SerialDevice::readData(char *data, qint64 maxSize)
{
    if(!isOpen())
    {
        setErrorString(m_errorStrings[0]); return 0;
    }

    qint64 size = qMin(m_readBuffer.size(), int(maxSize));
    qMemCopy(data, m_readBuffer.constData(), size);
    m_readBuffer.remove(0, size); return size;
}

qint64 SerialDevice::writeData(const char *data, qint64 maxSize)
{
    if(!isOpen())
    {
        setErrorString(m_errorStrings[0]); return 0;
    }

    m_writeBuffer.append(data, maxSize);
    emit bytesWritten(maxSize);
    return maxSize;
}

void SerialDevice::setupRunner()
{
    if(isOpen())
    {
        setup(); QTimer::singleShot(CALL_WAIT, this, SLOT(loopRunner()));
    }
}

void SerialDevice::loopRunner()
{
    if(isOpen())
    {
        loop(); QTimer::singleShot(CALL_WAIT, this, SLOT(loopRunner()));
    }
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
