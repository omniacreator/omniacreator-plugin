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
#include "newfiledialog.h"
#include "newprojectdialog.h"
#include "newfileorprojectdialog.h"
#include "openfileorprojectdialog.h"
#include "utilitempicker.h"
#include "utilpathpicker.h"

#include "omniacreator_global.h"

#include <utils/fileutils.h>
#include <utils/qtcassert.h>
#include <utils/proxyaction.h>

#include <extensionsystem/iplugin.h>
#include <extensionsystem/pluginmanager.h>

#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/documentmanager.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/imode.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/fancyactionbar.h>
#include <coreplugin/fancytabwidget.h>

// Device Includes
#include <projectexplorer/devicesupport/idevice.h>
#include <projectexplorer/devicesupport/idevicefactory.h>
#include <projectexplorer/devicesupport/idevicewidget.h>
#include <projectexplorer/devicesupport/devicemanager.h>

// Toolchain Includes
#include <projectexplorer/gcctoolchain.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/toolchainmanager.h>

// Versions Includes
#include <qtsupport/baseqtversion.h>
#include <qtsupport/qtversionmanager.h>

// Kit Includes
#include <projectexplorer/kit.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/kitmanager.h>

// Cmake Inlcludes
#include <cmakeprojectmanager/cmakeproject.h>
#include <cmakeprojectmanager/cmakeprojectmanager.h>
#include <cmakeprojectmanager/cmakeprojectconstants.h>
#include <cmakeprojectmanager/makestep.h>

// Project Includes
#include <projectexplorer/projectexplorersettings.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/session.h>
#include <projectexplorer/buildmanager.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/buildstep.h>

// Environment Includes

// Text Editor Cpp Includes
#include <texteditor/texteditorsettings.h>
#include <texteditor/generichighlighter/highlightersettings.h>
#include <texteditor/texteditorconstants.h>
#include <texteditor/texteditoractionhandler.h>
#include <cpptools/cppmodelmanagerinterface.h>
#include <cppeditor/cppeditorconstants.h>
#include <cpptools/cpptoolsconstants.h>

#define LABEL_INDEX 5

#define ICON_PATH ":icons/internal/icon/icon.png"
#define SPLASH_PATH ":icons/internal/about/about600x300.png"

#define CMAKE_PATH ":icons/external/cmake/cmake.png"

#ifdef Q_OS_WIN
    #define PATH_CHAR ";"
#else
    #define PATH_CHAR ":"
#endif

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

    void newFileOrProject();
    void openFileOrProject();

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

    void processMessage(const int &code);
    void errorMessage(const QString &text);

    // Begin Qt Creator Setup Functions ///////////////////////////////////////

    void deviceManagerSetup();
    void toolchainManagerSetup();
    void versionsManagerSetup();
    void kitManagerSetup();

    void cmakeManagerSetup();
    void projectManagerSetup();

    void environmentSetup();
    void textEditorCppSetup();

    // End Qt Creator Setup Functions /////////////////////////////////////////

    void updateRecentProjects();
    void updateRunCmake();

    void closeProject();
    void openProject();
    void cmakeChanged();

    void buildClicked();
    void runClicked();
    void reopenPort();

private:

    Q_DISABLE_COPY(OmniaCreatorPlugin)

    bool m_upload;

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

    // Begin Qt Creator Setup Variables ///////////////////////////////////////

    class MicrocontrollerDPSO :
    public ProjectExplorer::DeviceProcessSignalOperation
    {

    public:

        explicit MicrocontrollerDPSO() :
        ProjectExplorer::DeviceProcessSignalOperation() { }
        virtual ~MicrocontrollerDPSO() { }

        void killProcess(int pid)
        {
            Q_UNUSED(pid);
        }

        void killProcess(const QString &filePath)
        {
            Q_UNUSED(filePath);
        }

        void interruptProcess(int pid)
        {
            Q_UNUSED(pid);
        }

        void interruptProcess(const QString &filePath)
        {
            Q_UNUSED(filePath);
        }
    };

    class MicrocontrollerID :
    public ProjectExplorer::IDevice
    {

    public:

        explicit MicrocontrollerID() :
        ProjectExplorer::IDevice(MICROCONTROLLER_DEVICE_TYPE,
        ProjectExplorer::IDevice::AutoDetected,
        ProjectExplorer::IDevice::Hardware,
        MICROCONTROLLER_DEVICE_ID)
        {
            setDisplayName("Maker");
        }

        virtual ~MicrocontrollerID() { }

        ProjectExplorer::IDevice::DeviceInfo deviceInformation() const
        {
            return DeviceInfo();
        }

        virtual QString displayType() const
        {
            return "Microcontroller";
        }

        virtual ProjectExplorer::IDeviceWidget *createWidget()
        {
            return 0;
        }

        virtual QList<Core::Id> actionIds() const
        {
            return QList<Core::Id>();
        }

        virtual QString displayNameForActionId(Core::Id actionId) const
        {
            Q_UNUSED(actionId); return QString();
        }

        void executeAction(Core::Id actionId, QWidget *parent = 0)
        {
            Q_UNUSED(actionId); Q_UNUSED(parent);
        }

        ProjectExplorer::DeviceProcessSignalOperation::Ptr
        signalOperation() const
        {
            return ProjectExplorer::DeviceProcessSignalOperation::Ptr
            (new MicrocontrollerDPSO());
        }

        ProjectExplorer::IDevice::Ptr clone() const
        {
            return ProjectExplorer::IDevice::Ptr(new MicrocontrollerID(*this));
        }

        virtual QString qmlProfilerHost() const
        {
            return QString();
        }
    };

    class MicrocontrollerIDF :
    public ProjectExplorer::IDeviceFactory
    {

    public:

        explicit MicrocontrollerIDF(QObject *parent = NULL) :
        ProjectExplorer::IDeviceFactory(parent) { }
        virtual ~MicrocontrollerIDF() { }

        virtual QString displayNameForId(Core::Id type) const
        {
            return (type == MICROCONTROLLER_DEVICE_TYPE) ? "Maker" : "Other";
        }

        virtual QList<Core::Id> availableCreationIds() const
        {
            return QList<Core::Id>() << MICROCONTROLLER_DEVICE_TYPE;
        }

        virtual bool canCreate() const
        {
            return false;
        }

        virtual ProjectExplorer::IDevice::Ptr
        create(Core::Id id) const
        {
            Q_UNUSED(id); return ProjectExplorer::IDevice::Ptr();
        }

        virtual bool canRestore(const QVariantMap &map) const
        {
            return ProjectExplorer::IDevice::idFromMap(map) ==
            MICROCONTROLLER_DEVICE_ID;
        }

        virtual ProjectExplorer::IDevice::Ptr
        restore(const QVariantMap &map) const
        {
            QTC_ASSERT(canRestore(map),
            return ProjectExplorer::IDevice::Ptr());

            const ProjectExplorer::IDevice::Ptr device =
            ProjectExplorer::IDevice::Ptr(new MicrocontrollerID);

            device->fromMap(map);
            return device;
        }
    };

    class MicrocontrollerIDW :
    public ProjectExplorer::IDeviceWidget
    {

    public:

        explicit MicrocontrollerIDW
        (const ProjectExplorer::IDevice::Ptr &device, QWidget *parent = NULL) :
        ProjectExplorer::IDeviceWidget(device, parent) { }
        virtual ~MicrocontrollerIDW() { }

        virtual void updateDeviceFromUi() { }
    };

    ProjectExplorer::IDevice::Ptr m_device;
    ProjectExplorer::GccToolChain *m_toolchain;
    ProjectExplorer::Kit *m_kit;

    // End Qt Creator Setup Variables /////////////////////////////////////////

    static void messageHandler(QtMsgType type, const char *text);
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
