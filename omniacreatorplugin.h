/***************************************************************************//**
* @file
* Omnia Creator Plugin
*
* @version @n 1.0
* @date @n 6/29/2014
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2014 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 6/29/2014
*******************************************************************************/

#ifndef OMNIACREATORPLUGIN_H
#define OMNIACREATORPLUGIN_H

#include <QtCore>
#include <QtWidgets>

#include "serialmake.h"
#include "serialport.h"
#include "serialescape.h"
#include "utilitempicker.h"
#include "utilpathpicker.h"

#include "omniacreator_global.h"

#include <extensionsystem/iplugin.h>

#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>
#include <coreplugin/messagemanager.h>

#define LABEL_INDEX 5

#define ICON_PATH ":icons/internal/icon/icon.png"
#define SPLASH_PATH ":icons/internal/about/about600x300.png"

class OmniaCreatorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" \
                      FILE "OmniaCreator.json")

public:

    explicit OmniaCreatorPlugin();
    virtual ~OmniaCreatorPlugin();

    virtual bool initialize(const QStringList &arguments,
                            QString *errorString);

    virtual void extensionsInitialized();

    virtual bool delayedInitialize();

    virtual ExtensionSystem::IPlugin::ShutdownFlag aboutToShutdown();

private slots:

    void boardMenuAboutToShow();
    void widgetsMenuAboutToShow();

    void boardMenuTriggered(QAction *action);
    void widgetsMenuTriggered(QAction *action);

    void changeSerialPortBoardName();
    void changeSerialPortBoardType();

    void updateSerialPortMenu();
    void serialPortSelected(QAction *action);

    void updateBaudRateMenu();
    void baudRateSelected(QAction *action);

    void updateExportWidgetMenu();
    void updateRemoveWidgetMenu();

    void generalHelp();
    void editorHelp();

    void about();

    void errorMessage(const QString &text);

private:

    Q_DISABLE_COPY(OmniaCreatorPlugin)

    SerialMake *m_make;
    SerialPort *m_port;
    SerialEscape *m_escape;

    Core::ActionContainer *m_boardMenu;
    Core::ActionContainer *m_widgetsMenu;

    Core::Command *m_changeSerialPortBoardName;
    Core::Command *m_changeSerialPortBoardType;

    Core::ActionContainer *m_serialPortMenu;
    QLabel *m_serialPortMenuSelection;

    Core::ActionContainer *m_baudRateMenu;
    QLabel *m_baudRateMenuSelection;

    Core::ActionContainer *m_exportWidgetMenu;
    Core::Command *m_importWidget;

    Core::ActionContainer *m_removeWidgetMenu;
    Core::Command *m_removeAllWidgets;

    Core::Command *m_resetSerialPort;
    Core::Command *m_resetSerialTerminal;
};

#endif // OMNIACREATORPLUGIN_H

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
