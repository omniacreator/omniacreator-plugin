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

#ifndef SERIALMAKE_H
#define SERIALMAKE_H

#include <QtCore>
#include <QtGui>

#include "projectdefines.h"
#include "serialport.h"

#include "utilcolumndialog.h"

#define RED_CIRCLE ":icons/internal/circles/red/red24x24.png"
#define GREEN_CIRCLE ":icons/internal/circles/green/green24x24.png"

#define P_ROLE (Qt::UserRole + 2) // Path Role
#define I_ROLE (Qt::UserRole + 3) // Icon Role

class SerialPort;

class SerialMake : public QObject
{
    Q_OBJECT

public:

    static QString getCmakePath();
    static QString getNinjaPath();

    explicit SerialMake(QWidget *widget = NULL,
                        QSettings *settings = NULL, QObject *parent = NULL);

    virtual ~SerialMake();

    void setWidget(QWidget *widget);
    QWidget *getWidget() const;

    void setSettings(QSettings *settings);
    QSettings *getSettings() const;

    void setSerialPort(SerialPort *instance);
    SerialPort *getSerialPort() const;

    void setWorkspaceFolder(const QString &folder);
    QString getWorkspaceFolder() const;
    bool getWorkspaceFolderWasSet() const;
    void setWorkspaceFolderWasSet();

    void setProjectFolder(const QString &path);
    QString getProjectFolder() const;
    bool getProjectFolderWasSet() const;
    void setProjectFolderWasSet();
    QString getProjectFolderRelativeTo() const;

    void setProjectPortName(const QString &portName);
    QString getProjectPortName() const;
    bool getProjectPortNameWasSet() const;
    void setProjectPortNameWasSet();

    void setProjectMakeFile(const QString &makeFile);
    QString getProjectMakeFile() const;
    bool getProjectMakeFileWasSet() const;
    void setProjectMakeFileWasSet();
    QString getProjectMakeFileRelativeTo() const;

    QStringList getCMakeFilePaths() const;
    QStringList getSystemCMakeFilePaths() const;
    QStringList getUserCMakeFilePaths() const;

    QStringList getLibraryPaths() const;
    QStringList getSystemLibraryPaths() const;
    QStringList getUserLibraryPaths() const;

    QString getMakeSrcFolder() const;
    QString getMakeBuildFolder() const;
    QString getMakeFile() const;

    void updateProject();
    void updateProject2();

    void setMakeFileWithDialog();

private slots:

    void handlePortDestroyedOrChanged();

signals:

    void workspaceOrProjectSettingsChanged();

private:

    Q_DISABLE_COPY(SerialMake)

    friend class SerialPort;

    void setMakeFileWithDialogWithPort(const QString &portName,
                                       SerialPort *parent);

    void entryList(const QString &path, QStandardItem *parent);
    int indexOf(const QString &itemText, QStandardItem *parent);

    QModelIndex findItem(const QString &itemData,
                         QStandardItem *parent);

    QPointer<QWidget> m_widget;
    QPointer<QSettings> m_settings;
    QPointer<SerialPort> m_serialPort;

    QString m_makeSrcFolder;
    QString m_makeBuildFolder;
    QString m_makeFile;
};

#endif // SERIALMAKE_H

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
