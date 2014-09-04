/***************************************************************************//**
* @file
* Serial Make
*
* @version @n 1.0
* @date @n 6/10/2014
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2014 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 6/10/2014
*******************************************************************************/

#include "serialmake.h"

QString SerialMake::getCmakePath()
{
#if defined(Q_OS_WIN)
    QString path = "/../../../tools/cmake/bin/cmake.exe";
#else
    QString path = "/../../../tools/cmake/bin/cmake";
#endif

    return QDir::fromNativeSeparators(QDir::cleanPath(
    QApplication::applicationDirPath() + path));
}

QString SerialMake::getNinjaPath()
{
#if defined(Q_OS_WIN)
    QString path = "/ninja.exe";
#else
    QString path = "/ninja";
#endif

    return QDir::fromNativeSeparators(QDir::cleanPath(
    QApplication::applicationDirPath() + path));
}

SerialMake::SerialMake(QWidget *widget,
QSettings *settings, QObject *parent) : QObject(parent)
{
    m_widget = widget;
    m_settings = settings;

    updateProject();
}

SerialMake::~SerialMake()
{
    // UNUSED //
}

void SerialMake::setWidget(QWidget *widget)
{
    m_widget = widget;
}

QWidget *SerialMake::getWidget() const
{
    return m_widget;
}

void SerialMake::setSettings(QSettings *settings)
{
    m_settings = settings;
}

QSettings *SerialMake::getSettings() const
{
    return m_settings;
}

void SerialMake::setSerialPort(SerialPort *instance)
{
    if(m_serialPort != instance)
    {
        if(m_serialPort)
        {
            SerialPort *serialPort = m_serialPort; m_serialPort = NULL;

            serialPort->setSerialMake(NULL);

            disconnect(serialPort, SIGNAL(portDestroyedOrChanged()),
                       this, SLOT(handlePortDestroyedOrChanged()));
        }

        if(instance)
        {
            m_serialPort = instance;

            m_serialPort->setSerialMake(this);

            connect(m_serialPort, SIGNAL(portDestroyedOrChanged()),
                    this, SLOT(handlePortDestroyedOrChanged()));
        }
    }
}

SerialPort *SerialMake::getSerialPort() const
{
    return m_serialPort;
}

void SerialMake::setWorkspaceFolder(const QString &folder)
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    QString mFolder = QDir::fromNativeSeparators(QDir::cleanPath(folder));

    if(mFolder != getWorkspaceFolder())
    {
        settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
        settings.setValue(SERIAL_MAKE_KEY_WORKSPACE_FOLDER, mFolder);

        updateProject2();
    }
}

QString SerialMake::getWorkspaceFolder() const
{
    if(getWorkspaceFolderWasSet())
    {
        QSettings settings(m_settings ? m_settings->fileName() : QString(),
        m_settings ? m_settings->format() : QSettings::Format());

        settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
        return settings.value(SERIAL_MAKE_KEY_WORKSPACE_FOLDER).toString();
    }
    else
    {
        return QDir::fromNativeSeparators(QDir::cleanPath(
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
        QDir::separator() + QApplication::applicationName()));
    }
}

bool SerialMake::getWorkspaceFolderWasSet() const
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    return settings.contains(SERIAL_MAKE_KEY_WORKSPACE_FOLDER);
}

void SerialMake::setWorkspaceFolderWasSet()
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    settings.setValue(SERIAL_MAKE_KEY_WORKSPACE_FOLDER, getWorkspaceFolder());
}

void SerialMake::setProjectFolder(const QString &file)
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    QString mFile = QDir::fromNativeSeparators(QDir::cleanPath(file));

    if(mFile != getProjectFolder())
    {
        settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
        settings.setValue(SERIAL_MAKE_KEY_PROJECT_FOLDER, mFile);

        updateProject2();
    }
}

QString SerialMake::getProjectFolder() const
{
    if(getProjectFolderWasSet())
    {
        QSettings settings(m_settings ? m_settings->fileName() : QString(),
        m_settings ? m_settings->format() : QSettings::Format());

        settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
        return settings.value(SERIAL_MAKE_KEY_PROJECT_FOLDER).toString();
    }
    else
    {
        return QString();
    }
}

bool SerialMake::getProjectFolderWasSet() const
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    return settings.contains(SERIAL_MAKE_KEY_PROJECT_FOLDER);
}

void SerialMake::setProjectFolderWasSet()
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    settings.setValue(SERIAL_MAKE_KEY_PROJECT_FOLDER, getProjectFolder());
}

QString SerialMake::getProjectFolderRelativeTo() const
{
    foreach(QString path, getWorkspaceFolder())
    {
        if(getProjectFolder().startsWith(path, Qt::CaseInsensitive))
        {
            return QDir(path).relativeFilePath(getProjectFolder());
        }
    }

    return QString();
}

void SerialMake::setProjectPortName(const QString &portName)
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    QString mPortName = portName;

    if(mPortName != getProjectPortName())
    {
        settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
        settings.setValue(SERIAL_MAKE_KEY_PROJECT_PORT_NAME, mPortName);

        updateProject2();
    }
}

QString SerialMake::getProjectPortName() const
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    return settings.value(SERIAL_MAKE_KEY_PROJECT_PORT_NAME).toString();
}

bool SerialMake::getProjectPortNameWasSet() const
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    return settings.contains(SERIAL_MAKE_KEY_PROJECT_PORT_NAME);
}

void SerialMake::setProjectPortNameWasSet()
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    settings.setValue(SERIAL_MAKE_KEY_PROJECT_PORT_NAME, getProjectPortName());
}

void SerialMake::setProjectMakeFile(const QString &makeFile)
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    QString mMakeFile = QDir::fromNativeSeparators(QDir::cleanPath(makeFile));

    if(mMakeFile != getProjectMakeFile())
    {
        settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
        settings.setValue(SERIAL_MAKE_KEY_PROJECT_MAKE_FILE, mMakeFile);

        updateProject2();
    }
}

QString SerialMake::getProjectMakeFile() const
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    return settings.value(SERIAL_MAKE_KEY_PROJECT_MAKE_FILE).toString();
}

bool SerialMake::getProjectMakeFileWasSet() const
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    return settings.contains(SERIAL_MAKE_KEY_PROJECT_MAKE_FILE);
}

void SerialMake::setProjectMakeFileWasSet()
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    settings.setValue(SERIAL_MAKE_KEY_PROJECT_MAKE_FILE, getProjectMakeFile());
}

QString SerialMake::getProjectMakeFileRelativeTo() const
{
    foreach(QString path, getCMakeFilePaths())
    {
        if(getProjectMakeFile().startsWith(path, Qt::CaseInsensitive))
        {
            return QDir(path).relativeFilePath(getProjectMakeFile());
        }
    }

    return QString();
}

QStringList SerialMake::getCMakeFilePaths() const
{
    return getSystemCMakeFilePaths() + getUserCMakeFilePaths();
}

QStringList SerialMake::getSystemCMakeFilePaths() const
{
    QStringList list;

    list.append(QDir::fromNativeSeparators(QDir::cleanPath(
    QApplication::applicationDirPath() +
    "/../share/qtcreator/cmake-board-types")));

    return list;
}

QStringList SerialMake::getUserCMakeFilePaths() const
{
    QStringList list;

    list.append(QDir::fromNativeSeparators(QDir::cleanPath(
    getWorkspaceFolder() +
    "/cmake-board-types")));

    return list;
}

QStringList SerialMake::getLibraryPaths() const
{
    return getSystemLibraryPaths() + getUserLibraryPaths();
}

QStringList SerialMake::getSystemLibraryPaths() const
{
    QStringList list;

    list.append(QDir::fromNativeSeparators(QDir::cleanPath(
    QApplication::applicationDirPath() +
    "/../share/qtcreator/libraries")));

    return list;
}

QStringList SerialMake::getUserLibraryPaths() const
{
    QStringList list;

    list.append(QDir::fromNativeSeparators(QDir::cleanPath(
    getWorkspaceFolder() +
    "/libraries")));

    return list;
}

QString SerialMake::getMakeSrcFolder() const
{
    return m_makeSrcFolder;
}

QString SerialMake::getMakeBuildFolder() const
{
    return m_makeBuildFolder;
}

QString SerialMake::getMakeFile() const
{
    return m_makeFile;
}

void SerialMake::updateProject()
{
    QCryptographicHash hash(QCryptographicHash::Sha1);

    hash.addData(getWorkspaceFolder().toUtf8());
    hash.addData(getProjectFolder().toUtf8());
    hash.addData(getProjectPortName().toUtf8());
    hash.addData(getProjectMakeFile().toUtf8());

    QString tempLocation = QDir::fromNativeSeparators(QDir::cleanPath(
    QStandardPaths::writableLocation(QStandardPaths::TempLocation) +
    QDir::separator() + QApplication::applicationName() +
    QDir::separator() + "CMake" +
    QDir::separator() + hash.result().toHex().toUpper()));

    m_makeSrcFolder = QDir::fromNativeSeparators(QDir::cleanPath(
    tempLocation + QDir::separator() + "src"));

    m_makeBuildFolder = QDir::fromNativeSeparators(QDir::cleanPath(
    tempLocation + QDir::separator() + "build"));

    m_makeFile = QDir::fromNativeSeparators(QDir::cleanPath(
    m_makeSrcFolder + QDir::separator() + "CMakeLists.txt"));
}

void SerialMake::updateProject2()
{
    updateProject();

    if(!getProjectFolder().isEmpty())
    {
        if(!QDir().mkpath(m_makeSrcFolder))
        {
            QMessageBox::critical(m_widget, tr("Update Project"),
            tr("Unable to create cmake \"src\" directory!"));
            emit workspaceOrProjectSettingsChanged();
            return;
        }

        if(!QDir().mkpath(m_makeBuildFolder))
        {
            QMessageBox::critical(m_widget, tr("Update Project"),
            tr("Unable to create cmake \"build\" directory!"));
            emit workspaceOrProjectSettingsChanged();
            return;
        }

        QString text = "cmake_minimum_required(VERSION 2.8)\n\n";

        text.append("cmake_policy(VERSION 2.8)\n\n");

        text.append(QString("set(CMAKE_MAKE_PROGRAM \"%L1\")\n\n").
        arg(getNinjaPath()));

        text.append(QString("set(IDE_FOLDER \"%L1\")\n").
        arg(QDir::fromNativeSeparators(QDir::cleanPath(
        QApplication::applicationDirPath() +
        "/../../../ide"))));

        text.append(QString("set(TOOLS_FOLDER \"%L1\")\n\n").
        arg(QDir::fromNativeSeparators(QDir::cleanPath(
        QApplication::applicationDirPath() +
        "/../../../tools"))));

        text.append(QString("set(CMAKE_FILE_PATHS %L1)\n").
        arg('\"' + getCMakeFilePaths().join("\" \"") + '\"'));

        text.append(QString("set(LIBRARY_PATHS %L1)\n\n").
        arg('\"' + getLibraryPaths().join("\" \"") + '\"'));

        text.append(QString("set(PROJECT_FOLDER \"%L1\")\n\n").
        arg(getProjectFolder()));

        text.append(QString("set(SERIAL_PORT \"%L1\")\n\n").
        arg(getProjectPortName()));

        if(!getProjectMakeFile().isEmpty())
        {
            text.append("set(INCLUDE_SWITCH 0)\n");
            text.append(QString("include(\"%L1\")\n\n").
            arg(getProjectMakeFile()));
        }

        text.append(QString("project(\"%L1\")\n\n").
        arg(QDir(getProjectFolder()).dirName().
        replace(QRegularExpression("[^0-9A-Za-z]"),
        "_")));

        if(!getProjectMakeFile().isEmpty())
        {
            text.append("set(INCLUDE_SWITCH 1)\n");
            text.append(QString("include(\"%L1\")\n").
            arg(getProjectMakeFile()));
        }

        QFile file(m_makeFile);

        if(file.open(QIODevice::WriteOnly))
        {
            QByteArray utf8 = text.toUtf8();

            if(file.write(utf8) != utf8.size())
            {
                QMessageBox::critical(m_widget, tr("Update Project"),
                file.errorString());
            }
        }
        else
        {
            QMessageBox::critical(m_widget, tr("Update Project"),
            file.errorString());
        }
    }

    emit workspaceOrProjectSettingsChanged();
}

void SerialMake::handlePortDestroyedOrChanged()
{
    QString portName = m_serialPort->getPortName();

    setProjectPortName(portName);

    if(!portName.isEmpty())
    {
        setProjectMakeFile(m_serialPort->getMakeFile(portName));
    }
}

void SerialMake::setMakeFileWithDialog()
{
    // Setup Model ////////////////////////////////////////////////////////////

    QStandardItemModel makeFilePathsModel;

    foreach(const QString &path, getCMakeFilePaths())
    {
        entryList(path, makeFilePathsModel.invisibleRootItem());
    }

    QStandardItem *item = new QStandardItem(tr("None"));
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    item->setIcon(QIcon(RED_CIRCLE));

    makeFilePathsModel.invisibleRootItem()->appendRow(item);

    // Setup Dialog ///////////////////////////////////////////////////////////

    UtilColumnDialog dialog(tr("Change Board Type"), m_widget);
    dialog.setText(tr("Please select a board type"));

    dialog.setModel(&makeFilePathsModel);
    dialog.setCurrentIndex(findItem(getProjectMakeFile(),
    makeFilePathsModel.invisibleRootItem()));

    if(dialog.exec() == QDialog::Accepted)
    {
        setProjectMakeFile(dialog.getCurrentIndex().data(P_ROLE).toString());
    }
}

void SerialMake::setMakeFileWithDialogWithPort(const QString &portName,
                                               SerialPort *parent)
{
    // Setup Model ////////////////////////////////////////////////////////////

    QStandardItemModel makeFilePathsModel;

    foreach(const QString &path, getCMakeFilePaths())
    {
        entryList(path, makeFilePathsModel.invisibleRootItem());
    }

    QStandardItem *item = new QStandardItem(tr("None"));
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    item->setIcon(QIcon(RED_CIRCLE));

    makeFilePathsModel.invisibleRootItem()->appendRow(item);

    // Setup Dialog ///////////////////////////////////////////////////////////

    UtilColumnDialog dialog(tr("Change Board Type"), parent->getWidget());
    dialog.setText(tr("Please select a board type"));

    dialog.setModel(&makeFilePathsModel);
    dialog.setCurrentIndex(findItem(parent->getMakeFile(portName),
    makeFilePathsModel.invisibleRootItem()));

    QIODevice *port = parent->getPort(portName);

    if(port)
    {
        connect(port, SIGNAL(aboutToClose()), &dialog, SLOT(reject()));
    }

    if(dialog.exec() == QDialog::Accepted)
    {
        parent->setMakeFile(portName,
        dialog.getCurrentIndex().data(P_ROLE).toString());
    }
}

void SerialMake::entryList(const QString &path, QStandardItem *parent)
{
    foreach(const QString &itemName,
    QDir(path).entryList(QStringList() << "CMakeLists.txt" << "*.cmake",
    QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot,
    QDir::Name | QDir::DirsFirst | QDir::LocaleAware))
    {
        QString baseName =
        QFileInfo(path + QDir::separator() + itemName).isDir() ?
        itemName : QFileInfo(itemName).completeBaseName();
        int modelIndex = indexOf(baseName, parent);

        QStandardItem *item;

        if(modelIndex == -1)
        {
            parent->appendRow(item = new QStandardItem(baseName));
        }
        else
        {
            item = parent->child(modelIndex);
        }

        item->setData(QString(path + QDir::separator() + itemName), P_ROLE);
        item->setData(parent->data(I_ROLE), I_ROLE);

        if(QFileInfo(path+QDir::separator()+baseName+".bmp").exists())
        {
            item->setData(QString(path+QDir::separator()+baseName+".bmp"),
                          I_ROLE);
        }
        else if(QFileInfo(path+QDir::separator()+baseName+".jpg").exists())
        {
            item->setData(QString(path+QDir::separator()+baseName+".jpg"),
                          I_ROLE);
        }
        else if(QFileInfo(path+QDir::separator()+baseName+".jpeg").exists())
        {
            item->setData(QString(path+QDir::separator()+baseName+".jpeg"),
                          I_ROLE);
        }
        else if(QFileInfo(path+QDir::separator()+baseName+".png").exists())
        {
            item->setData(QString(path+QDir::separator()+baseName+".png"),
                          I_ROLE);
        }

        if(QFileInfo(item->data(P_ROLE).toString()).isDir())
        {
            item->setFlags(Qt::ItemIsEnabled);
            item->setIcon(QFileIconProvider().icon(QFileIconProvider::Folder));

            entryList(item->data(P_ROLE).toString(), item);
        }
        else
        {
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            item->setIcon(QIcon(GREEN_CIRCLE));
        }
    }
}

int SerialMake::indexOf(const QString &itemText, QStandardItem *parent)
{
    for(int i = 0, j = parent->rowCount(); i < j; i++)
    {
        if(parent->child(i)->text() == itemText)
        {
            return i;
        }
    }

    return -1;
}

QModelIndex SerialMake::findItem(const QString &itemData,
                                 QStandardItem *parent)
{
    for(int i = 0, j = parent->rowCount(); i < j; i++)
    {
        if(QFileInfo(parent->child(i)->data(P_ROLE).toString()).isDir())
        {
            QModelIndex modelIndex = findItem(itemData, parent->child(i));

            if(modelIndex.isValid())
            {
                return modelIndex;
            }
        }

        if(QDir(parent->child(i)->data(P_ROLE).toString()) == QDir(itemData))
        {
            return parent->child(i)->index();
        }
    }

    return QModelIndex();
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
