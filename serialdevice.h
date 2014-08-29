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

#ifndef SERIAL_DEVICE_H
#define SERIAL_DEVICE_H

#include <QtCore>

#define CALL_WAIT 20 // 50 hz

class SerialDevice : public QIODevice
{
    Q_OBJECT

public:

    explicit SerialDevice(QObject *parent = NULL);
    virtual ~SerialDevice();

    virtual bool open(OpenMode mode);
    virtual void close();

    static void clearInput();
    static void clearOutput();

    virtual bool atEnd() const;
    virtual bool canReadLine() const;
    virtual bool isSequential() const;
    virtual qint64 bytesAvailable() const;
    virtual qint64 bytesToWrite() const;

    static bool atEndExtern();
    static bool canReadLineExtern();
    static bool isSequentialExtern();
    static qint64 bytesAvailableExtern();
    static qint64 bytesToWriteExtern();

    static qint64 millisecondsExtern();
    static void waitMillisecondsExtern(qint64 milliseconds);

    static void resetEventExtern();

    static qint64 readDataExtern(char *data);
    static qint64 writeDataExtern(char *data);
    static qint64 readDataExtern(char *data, qint64 maxSize);
    static qint64 writeDataExtern(const char *data, qint64 maxSize);

protected:

    virtual void setup() = 0;
    virtual void loop() = 0;

    virtual qint64 readData(char *data, qint64 maxSize);
    virtual qint64 writeData(const char *data, qint64 maxSize);

private slots:

    void setupRunner();
    void loopRunner();

signals:

    void resetEvent();

private:

    Q_DISABLE_COPY(SerialDevice)

    static QByteArray m_readBuffer;
    static QByteArray m_writeBuffer;

    static SerialDevice *m_port;

    static const char *m_errorStrings[];
};

#endif // SERIAL_DEVICE_H

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
