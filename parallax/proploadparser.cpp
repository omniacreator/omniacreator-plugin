/***************************************************************************//**
* @file
* Prop Load Parser
*
* @version @n 1.0
* @date @n 12/3/2014
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2014 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 12/3/2014
*******************************************************************************/

#include "proploadparser.h"

PropLoadParser::PropLoadParser() : ProjectExplorer::IOutputParser()
{
    m_regex = QRegularExpression("^error: (.+)$");
}

void PropLoadParser::stdError(const QString &line)
{
    QRegularExpressionMatch match = m_regex.match(rightTrimmed(line));

    if(match.hasMatch())
    {
        QString text = match.captured(1);
        text[0] = text.at(0).toUpper();

        QMessageBox *mBox = new QMessageBox(QMessageBox::Critical,
        QApplication::applicationName(), text, QMessageBox::Ok,
        Core::ICore::mainWindow());

        connect(mBox, SIGNAL(finished(int)), mBox, SLOT(deleteLater()));
        QTimer::singleShot(0, mBox, SLOT(exec()));
    }
    else
    {
        IOutputParser::stdError(line);
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
