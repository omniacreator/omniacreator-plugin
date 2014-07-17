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

SerialMake::SerialMake(QWidget *widget,
QSettings *settings, QObject *parent) : QObject(parent)
{
    m_widget = widget;
    m_settings = settings;

    updateProject();
}

SerialMake::~SerialMake()
{
    m_updateLock.tryLock();
    m_updateLock.unlock();
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

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    settings.setValue(SERIAL_MAKE_KEY_WORKSPACE_FOLDER, folder);

    // Start QtCreator Integration Stuff //////////////////////////////////////
    Core::DocumentManager::setProjectsDirectory(folder);
    // End QtCreator Integration Stuff ////////////////////////////////////////

    emit workspaceOrProjectSettingsChanged(); updateProject();
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
        return QDir::toNativeSeparators(QDir::cleanPath(
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

void SerialMake::setProjectFile(const QString &file)
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    settings.setValue(SERIAL_MAKE_KEY_PROJECT_FILE, file);

    emit workspaceOrProjectSettingsChanged(); updateProject();
}

QString SerialMake::getProjectFile() const
{
    if(getProjectFileWasSet())
    {
        QSettings settings(m_settings ? m_settings->fileName() : QString(),
        m_settings ? m_settings->format() : QSettings::Format());

        settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
        return settings.value(SERIAL_MAKE_KEY_PROJECT_FILE).toString();
    }
    else
    {
        return QDir::toNativeSeparators(QDir::cleanPath(QString()));
    }
}

QString SerialMake::getProjectFile2() const
{
    return QDir::fromNativeSeparators(getProjectFile());
}

bool SerialMake::getProjectFileWasSet() const
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    return settings.contains(SERIAL_MAKE_KEY_PROJECT_FILE);
}

void SerialMake::setProjectFileWasSet()
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    settings.setValue(SERIAL_MAKE_KEY_PROJECT_FILE, getProjectFile());
}

void SerialMake::setProjectPortName(const QString &portName)
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    settings.setValue(SERIAL_MAKE_KEY_PROJECT_PORT_NAME, portName);

    emit workspaceOrProjectSettingsChanged(); updateProject();
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

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    settings.setValue(SERIAL_MAKE_KEY_PROJECT_MAKE_FILE, makeFile);

    emit workspaceOrProjectSettingsChanged(); updateProject();
}

QString SerialMake::getProjectMakeFile() const
{
    QSettings settings(m_settings ? m_settings->fileName() : QString(),
    m_settings ? m_settings->format() : QSettings::Format());

    settings.beginGroup(SERIAL_MAKE_KEY_GROUP);
    return settings.value(SERIAL_MAKE_KEY_PROJECT_MAKE_FILE).toString();
}

QString SerialMake::getProjectMakeFile2() const
{
    return QDir::fromNativeSeparators(getProjectMakeFile());
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

QStringList SerialMake::getCMakeFilePaths() const
{
    return getSystemCMakeFilePaths() + getUserCMakeFilePaths();
}

QStringList SerialMake::getCMakeFilePaths2() const
{
    QStringList list;

    foreach(const QString &path, getCMakeFilePaths())
    {
        list.append(QDir::fromNativeSeparators(path));
    }

    return list;
}

QStringList SerialMake::getSystemCMakeFilePaths() const
{
    QStringList list;

    list.append(QDir::toNativeSeparators(QDir::cleanPath(
    QApplication::applicationDirPath() +
    "/../share/qtcreator/cmake-board-types")));

    return list;
}

QStringList SerialMake::getUserCMakeFilePaths() const
{
    QStringList list;

    list.append(QDir::toNativeSeparators(QDir::cleanPath(
    getWorkspaceFolder() +
    "/cmake-board-types")));

    return list;
}

QStringList SerialMake::getLibraryPaths() const
{
    return getSystemLibraryPaths() + getUserLibraryPaths();
}

QStringList SerialMake::getLibraryPaths2() const
{
    QStringList list;

    foreach(const QString &path, getLibraryPaths())
    {
        list.append(QDir::fromNativeSeparators(path));
    }

    return list;
}

QStringList SerialMake::getSystemLibraryPaths() const
{
    QStringList list;

    list.append(QDir::toNativeSeparators(QDir::cleanPath(
    QApplication::applicationDirPath() +
    "/../share/qtcreator/libraries")));

    return list;
}

QStringList SerialMake::getUserLibraryPaths() const
{
    QStringList list;

    list.append(QDir::toNativeSeparators(QDir::cleanPath(
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

void SerialMake::handlePortDestroyedOrChanged()
{
    QString portName = m_serialPort->getPortName();

    setProjectPortName(portName);

    if(!portName.isEmpty())
    {
        setProjectMakeFile(m_serialPort->getMakeFile(portName));
    }
}

void SerialMake::updateProject()
{
    if(m_updateLock.tryLock())
    {
        QTimer::singleShot(PROJECT_TIMEOUT, this, SLOT(setupProject()));
    }
}

void SerialMake::setupProject()
{
    // Setup Build Folder /////////////////////////////////////////////////////

    QCryptographicHash hash(QCryptographicHash::Sha1);

    hash.addData(getWorkspaceFolder().toUtf8());
    hash.addData(getProjectFile().toUtf8());
    hash.addData(getProjectPortName().toUtf8());
    hash.addData(getProjectMakeFile().toUtf8());

    QString tempLocation = QDir::toNativeSeparators(QDir::cleanPath(
    QStandardPaths::writableLocation(QStandardPaths::TempLocation) +
    QDir::separator() + QApplication::applicationName() +
    QDir::separator() + "CMake" +
    QDir::separator() + hash.result().toHex().toUpper()));

    m_makeSrcFolder = tempLocation + QDir::separator() + "src";
    m_makeBuildFolder = tempLocation + QDir::separator() + "build";

    // Start QtCreator Integration Stuff //////////////////////////////////////
    Core::DocumentManager::setBuildDirectory(m_makeBuildFolder);
    // End QtCreator Integration Stuff ////////////////////////////////////////

    if(!QDir().mkpath(m_makeSrcFolder))
    {
        QMessageBox::critical(m_widget, tr("Setup Project"),
        tr("Unable to create cmake \"src\" directory!"));
        m_updateLock.unlock(); return;
    }

    if(!QDir().mkpath(m_makeBuildFolder))
    {
        QMessageBox::critical(m_widget, tr("Setup Project"),
        tr("Unable to create cmake \"build\" directory!"));
        m_updateLock.unlock(); return;
    }

    // Setup Build File ///////////////////////////////////////////////////////

    QFile file(m_makeSrcFolder + QDir::separator() + "CMakeLists.txt");

    if(file.open(QIODevice::ReadWrite))
    {
        file.write("cmake_minimum_required(VERSION 2.8)\n\n");

        file.write(QString("set(IDE_FOLDER \"%L1\")\n").
        arg(QDir::fromNativeSeparators(QDir::cleanPath( // '\' -> '/'
        QApplication::applicationDirPath() +
        "/../../../ide"))).toUtf8());

        file.write(QString("set(TOOLS_FOLDER \"%L1\")\n\n").
        arg(QDir::fromNativeSeparators(QDir::cleanPath( // '\' -> '/'
        QApplication::applicationDirPath() +
        "/../../../tools"))).toUtf8());

        file.write(QString("set(CMAKE_FILE_PATHS %L1)\n").
        arg('\"' + getCMakeFilePaths2().join("\" \"") + '\"').toUtf8());

        file.write(QString("set(LIBRARY_PATHS %L1)\n\n").
        arg('\"' + getLibraryPaths2().join("\" \"") + '\"').toUtf8());

        file.write(QString("set(PROJECT_FILE \"%L1\")\n\n").
        arg(getProjectFile2()).toUtf8());
        file.write(QString("set(SERIAL_PORT \"%L1\")\n\n").
        arg(getProjectPortName()).toUtf8());

        file.write("set(BEFORE_PROJECT_COMMAND true)\n");
        file.write(QString("include(\"%L1\")\n\n").
        arg(getProjectMakeFile2()).toUtf8());

        if(getProjectFile2().isEmpty())
        {
            file.write("project(Default C CXX)\n\n");
        }
        else
        {
            file.write(QString("project(%L1 C CXX)\n\n").
            arg(QFileInfo(getProjectFile2()).baseName().
            replace(QRegularExpression("[^0-9A-Za-z]"), "_")).toUtf8());
        }

        file.write("set(AFTER_PROJECT_COMMAND true)\n");
        file.write(QString("include(\"%L1\")\n").
        arg(getProjectMakeFile2()).toUtf8());

        file.close();
    }
    else
    {
        QMessageBox::critical(m_widget, tr("Setup Project"),
        tr("Unable to create cmake lists file!"));
        m_updateLock.unlock(); return;
    }

    m_updateLock.unlock();
}

void SerialMake::setupProject2()
{

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
        itemName : QFileInfo(itemName).baseName();
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
