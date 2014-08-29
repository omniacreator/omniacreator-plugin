/***************************************************************************//**
* @file
* Open File or Project Dialog
*
* @version @n 1.0
* @date @n 7/22/2014
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2014 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 7/22/2014
*******************************************************************************/

#ifndef OPENFILEORPROJECTDIALOG_H
#define OPENFILEORPROJECTDIALOG_H

#include <QtCore>
#include <QtWidgets>

namespace Ui
{
    class OpenFileOrProjectDialog;
}

class OpenFileOrProjectDialog : public QDialog
{
    Q_OBJECT

public:

    explicit OpenFileOrProjectDialog(const QString &title,QWidget*parent=NULL);
    virtual ~OpenFileOrProjectDialog();

    enum
    {
        openFileWasPressed = 2, openProjectWasPressed = 3
    };

public slots:

    void openFilePressed();
    void openProjectPressed();

private:

    Q_DISABLE_COPY(OpenFileOrProjectDialog)

    Ui::OpenFileOrProjectDialog *m_ui;
};

#endif // OPENFILEORPROJECTDIALOG_H

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