/***************************************************************************//**
* @file
* Open Spin Parser
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

#include "openspinparser.h"

OpenSpinParser::OpenSpinParser() : ProjectExplorer::IOutputParser()
{
    m_regex = QRegularExpression("(.+?)\\((\\d+):(\\d+)\\) : error : (.+)");
}

void OpenSpinParser::stdError(const QString &line)
{
    QRegularExpressionMatch match = m_regex.match(rightTrimmed(line));

    if(match.hasMatch())
    {
        Utils::FileName fileName =
        Utils::FileName::fromUserInput(match.captured(1));

        int lineNumber = match.captured(2).toInt();

        int columnNumber = match.captured(3).toInt();
        Q_UNUSED(columnNumber);

        QString errorMessage = match.captured(4);

        emit addTask(ProjectExplorer::Task(ProjectExplorer::Task::Error,
        errorMessage, fileName, lineNumber,
        ProjectExplorer::Constants::TASK_CATEGORY_COMPILE));
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
