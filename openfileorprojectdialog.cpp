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

#include "openfileorprojectdialog.h"
#include "ui_openfileorprojectdialog.h"

OpenFileOrProjectDialog::OpenFileOrProjectDialog(const QString &title,
QWidget *parent) : QDialog(parent), m_ui(new Ui::OpenFileOrProjectDialog)
{
    m_ui->setupUi(this); setWindowTitle(title); setObjectName(title);

    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::Dialog);

    connect(m_ui->openFileButton, SIGNAL(clicked()),
            this, SLOT(openFilePressed()));

    connect(m_ui->openProjectButton, SIGNAL(clicked()),
            this, SLOT(openProjectPressed()));

    connect(m_ui->openProjectFileButton, SIGNAL(clicked()),
            this, SLOT(openProjectFilePressed()));
}

OpenFileOrProjectDialog::~OpenFileOrProjectDialog()
{
    delete m_ui;
}

void OpenFileOrProjectDialog::openFilePressed()
{
    QDialog::done(openFileWasPressed);
}

void OpenFileOrProjectDialog::openProjectPressed()
{
    QDialog::done(openProjectWasPressed);
}

void OpenFileOrProjectDialog::openProjectFilePressed()
{
    QDialog::done(openProjectFileWasPressed);
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
