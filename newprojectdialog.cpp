/***************************************************************************//**
* @file
* New Project Dialog
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

#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

NewProjectDialog::NewProjectDialog(const QString &title,
const QString &name, const QString &defaultDir, QWidget *parent) :
QDialog(parent), m_ui(new Ui::NewProjectDialog)
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

    connect(m_ui->groupBox, SIGNAL(clicked()),
            this, SLOT(userInterfaceChanged()));

    connect(m_ui->cButton, SIGNAL(clicked()),
            this, SLOT(userInterfaceChanged()));

    connect(m_ui->cppButton, SIGNAL(clicked()),
            this, SLOT(userInterfaceChanged()));

    connect(m_ui->inoButton, SIGNAL(clicked()),
            this, SLOT(userInterfaceChanged()));

    setName(name);
    setDefaultDir(defaultDir);
}

NewProjectDialog::~NewProjectDialog()
{
    delete m_ui;
}

void NewProjectDialog::setName(const QString &name)
{
    m_ui->nameEdit->setText(name);
}

QString NewProjectDialog::getName() const
{
    return m_ui->nameEdit->text();
}

void NewProjectDialog::setDefaultDir(const QString &defaultDir)
{
    m_ui->edit->setText(defaultDir);
}

QString NewProjectDialog::getDefaultDir() const
{
    return m_ui->edit->text();
}

bool NewProjectDialog::getCreateMainFiles()
{
    return m_ui->groupBox->isChecked();
}

bool NewProjectDialog::getCreateCMainFile()
{
    return m_ui->cButton->isChecked();
}

bool NewProjectDialog::getCreateCppMainFile()
{
    return m_ui->cppButton->isChecked();
}

bool NewProjectDialog::getCreateArduinoMainFile()
{
    return m_ui->inoButton->isChecked();
}

void NewProjectDialog::pick()
{
    QString defaultDir = QFileDialog::getExistingDirectory(this, windowTitle(),
                                                           m_ui->edit->text());

    if(!defaultDir.isEmpty())
    {
        m_ui->edit->setText(defaultDir);
    }
}

void NewProjectDialog::textChanged(const QString &text)
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

void NewProjectDialog::userInterfaceChanged()
{
    if(!getName().isEmpty())
    {
        if(!QFileInfo(getDefaultDir(), getName()).exists())
        {
            if(QFileInfo(getDefaultDir()).isAbsolute())
            {
                if(QFileInfo(getDefaultDir()).isDir())
                {
                    if(getCreateMainFiles())
                    {
                        if(getCreateCMainFile())
                        {
                            m_ui->label->setText(tr(
                            "\nAbout to create project folder: \"%L1\"\n"
                            "In directory \"%L2\"\n\n"
                            "About to create files: \"%L1.c\" & \"%L1.h\"\n"
                            "In directory \"%L3\"\n"
                            ).arg(getName()).arg(getDefaultDir()).
                            arg(QDir::fromNativeSeparators(QDir::cleanPath(
                            getDefaultDir() + QDir::separator() +
                            getName()))));
                        }
                        else if(getCreateCppMainFile())
                        {
                            m_ui->label->setText(tr(
                            "\nAbout to create project folder: \"%L1\"\n"
                            "In directory \"%L2\"\n\n"
                            "About to create files: \"%L1.cpp\" & \"%L1.h\"\n"
                            "In directory \"%L3\"\n"
                            ).arg(getName()).arg(getDefaultDir()).
                            arg(QDir::fromNativeSeparators(QDir::cleanPath(
                            getDefaultDir() + QDir::separator() +
                            getName()))));
                        }
                        else if(getCreateArduinoMainFile())
                        {
                            m_ui->label->setText(tr(
                            "\nAbout to create project folder: \"%L1\"\n"
                            "In directory \"%L2\"\n\n"
                            "About to create file: \"%L1.ino\"\n"
                            "In directory \"%L3\"\n"
                            ).arg(getName()).arg(getDefaultDir()).
                            arg(QDir::fromNativeSeparators(QDir::cleanPath(
                            getDefaultDir() + QDir::separator() +
                            getName()))));
                        }
                        else
                        {
                            Q_UNREACHABLE();
                        }
                    }
                    else
                    {
                        m_ui->label->setText(tr(
                        "\nAbout to create project folder: \"%L1\"\n"
                        "In directory \"%L2\"\n"
                        ).arg(getName()).arg(getDefaultDir()));
                    }
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
            "\nFolder already exists...\n"));
        }
    }
    else
    {
        m_ui->label->setText(tr(
        "\nName is empty...\n"));
    }

    adjustSize();
}

void NewProjectDialog::showEvent(QShowEvent *event)
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
