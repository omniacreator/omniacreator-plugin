/***************************************************************************//**
* @file
* New File Dialog
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

#include "newfiledialog.h"
#include "ui_newfiledialog.h"

NewFileDialog::NewFileDialog(const QString &title,
const QString &name, const QString &defaultDir, QWidget *parent) :
QDialog(parent), m_ui(new Ui::NewFileDialog)
{
    m_ui->setupUi(this); setWindowTitle(title); setObjectName(title);

    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::Dialog);

    m_ui->button->setIcon(QFileIconProvider().icon(QFileIconProvider::Folder));

    connect(m_ui->button, SIGNAL(clicked()),
            this, SLOT(pick()));

    connect(m_ui->nameEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textChanged(QString)));

    connect(m_ui->nameEdit, SIGNAL(textChanged(QString)),
            this, SLOT(userInterfaceChanged()));

    connect(m_ui->edit, SIGNAL(textChanged(QString)),
            this, SLOT(textChanged(QString)));

    connect(m_ui->edit, SIGNAL(textChanged(QString)),
            this, SLOT(userInterfaceChanged()));

    setName(name);
    setDefaultDir(defaultDir);
}

NewFileDialog::~NewFileDialog()
{
    delete m_ui;
}

void NewFileDialog::setName(const QString &name)
{
    m_ui->nameEdit->setText(name);
}

QString NewFileDialog::getName() const
{
    return m_ui->nameEdit->text();
}

void NewFileDialog::setDefaultDir(const QString &defaultDir)
{
    m_ui->edit->setText(defaultDir);
}

QString NewFileDialog::getDefaultDir() const
{
    return m_ui->edit->text();
}

void NewFileDialog::pick()
{
    QString defaultDir = QFileDialog::getExistingDirectory(this, windowTitle(),
                                                           m_ui->edit->text());

    if(!defaultDir.isEmpty())
    {
        m_ui->edit->setText(defaultDir);
    }
}

void NewFileDialog::textChanged(const QString &text)
{
    if(sender() == m_ui->nameEdit)
    {
        bool ok = (!text.isEmpty()) &&
        (!QFileInfo(getDefaultDir(), text).exists());

        m_ui->box->button(QDialogButtonBox::Ok)->setDefault(ok);
        m_ui->box->button(QDialogButtonBox::Ok)->setEnabled(ok);
    }
    else
    {
        bool ok = QFileInfo(text).isAbsolute() && QFileInfo(text).isDir();

        m_ui->box->button(QDialogButtonBox::Ok)->setDefault(ok);
        m_ui->box->button(QDialogButtonBox::Ok)->setEnabled(ok);
    }
}

void NewFileDialog::userInterfaceChanged()
{
    if(!getName().isEmpty())
    {
        if(!QFileInfo(getDefaultDir(), getName()).exists())
        {
            if(QFileInfo(getDefaultDir()).isAbsolute())
            {
                if(QFileInfo(getDefaultDir()).isDir())
                {
                    m_ui->label->setText(tr(
                    "\nAbout to create file: \"%L1\"\n"
                    "In directory \"%L2\"\n"
                    ).arg(getName()).arg(getDefaultDir()));
                }
                else
                {
                    m_ui->label->setText(tr(
                    "\nPath is not a folder...\n"));
                }
            }
            else
            {
                m_ui->label->setText(tr(
                "\nPath is not absolute...\n"));
            }
        }
        else
        {
            m_ui->label->setText(tr(
            "\nFile already exists...\n"));
        }
    }
    else
    {
        m_ui->label->setText(tr(
        "\nName is empty...\n"));
    }

    adjustSize();
}

void NewFileDialog::showEvent(QShowEvent *event)
{
    m_ui->nameEdit->setFocus();

    QDialog::showEvent(event);
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
