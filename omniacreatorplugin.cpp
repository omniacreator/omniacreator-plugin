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

#include "omniacreatorplugin.h"

OmniaCreatorPlugin::OmniaCreatorPlugin()
{
    // Help QtCreator find Ninja...
    // See CMakeProjectManager->CMakeManager
    QString path = qgetenv("PATH");
    path.append(PATH_CHAR + QApplication::applicationDirPath());
    qputenv("PATH", path.toUtf8());

    // Remove warnings from files in application folder tree...
    // See CMakeProjectManager->CMakeParser
    // See ProjectExplorer->GccParser
    // See ProjectExplorer->GnuMakeParser
    // See ProjectExplorer->LdParser
    QString wno_path = qgetenv("WNO_PATH");
    wno_path.append(',' + QDir::fromNativeSeparators(QDir::cleanPath(
    QApplication::applicationDirPath() + QDir::separator() + "../../..")));
    qputenv("WNO_PATH", wno_path.toUtf8());

    ///////////////////////////////////////////////////////////////////////////

    QSettings settings(Core::ICore::settings()->fileName(),
                       Core::ICore::settings()->format());

    settings.beginGroup(PLUGIN_DIALOG_KEY_GROUP);

    if(!settings.value(PLUGIN_DIALOG_KEY_INITIAL_SETTINGS, false).toBool())
    {
        QSettings initSettings(Core::ICore::settings()->fileName(),
                               Core::ICore::settings()->format());

        initSettings.beginGroup("Navigation");

        initSettings.setValue("Views",
        QStringList() << "File System" << "Bookmarks" << "Open Documents");

        initSettings.setValue("Visible",
        false);

        initSettings.endGroup();

        ///////////////////////////////////////////////////////////////////////



        ///////////////////////////////////////////////////////////////////////

        settings.setValue(PLUGIN_DIALOG_KEY_INITIAL_SETTINGS, true);
    }

    settings.endGroup();
}

OmniaCreatorPlugin::~OmniaCreatorPlugin()
{
    // Help QtCreator find Ninja...
    // See CMakeProjectManager->CMakeManager
    QString path = qgetenv("PATH");
    path.remove(PATH_CHAR + QApplication::applicationDirPath());
    qputenv("PATH", path.toUtf8());

    // Remove warnings from files in application folder tree...
    // See CMakeProjectManager->CMakeParser
    // See ProjectExplorer->GccParser
    // See ProjectExplorer->GnuMakeParser
    // See ProjectExplorer->LdParser
    QString wno_path = qgetenv("WNO_PATH");
    wno_path.remove(',' + QDir::fromNativeSeparators(QDir::cleanPath(
    QApplication::applicationDirPath() + QDir::separator() + "../../..")));
    qputenv("WNO_PATH", wno_path.toUtf8());
}

bool OmniaCreatorPlugin::initialize(const QStringList &arguments,
                                    QString *errorString)
{
    Q_UNUSED(arguments); Q_UNUSED(errorString);

    ///////////////////////////////////////////////////////////////////////////

    int labelIndex = LABEL_INDEX;

    // Application Reskining //////////////////////////////////////////////////

    QApplication::setOrganizationName(PROJECT_VENDOR_STR);
    QApplication::setOrganizationDomain(PROJECT_DOMAIN_NAME_STR);
    QApplication::setApplicationName(PROJECT_FULL_NAME_STR);
    QApplication::setApplicationVersion(PROJECT_VERSION_STR);

    QApplication::setWindowIcon(QIcon(ICON_PATH));

    // Splash Screen //////////////////////////////////////////////////////////

    QSplashScreen *splashScreen =
    new QSplashScreen(QPixmap(SPLASH_PATH));

    connect(Core::ICore::instance(), SIGNAL(coreOpened()),
            splashScreen, SLOT(close()));

    splashScreen->show();

    SerialOscilloscope::initFftw(Core::ICore::settings(), splashScreen);

    // Serial Make Init ///////////////////////////////////////////////////////

    m_make = new SerialMake(Core::ICore::mainWindow(),
                            Core::ICore::settings(), this);

    m_upload = false;

    qputenv("OC_WORKSPACE_FOLDER", m_make->getWorkspaceFolder().toUtf8());
    qputenv("OC_PROJECT_FOLDER", m_make->getProjectFolder().toUtf8());
    qputenv("OC_SERIAL_PORT", m_make->getProjectPortName().toUtf8());
    qputenv("OC_MAKE_FILE", m_make->getProjectMakeFile().toUtf8());

    connect(m_make, SIGNAL(workspaceOrProjectSettingsChanged()),
            this, SLOT(cmakeChanged()));

    // Serial Port Init ///////////////////////////////////////////////////////

    m_port = new SerialPort(Core::ICore::mainWindow(),
                            Core::ICore::settings(), this);

    m_port->setSerialMake(m_make);

    connect(m_port, SIGNAL(demoPortListChanged()),
            this, SLOT(boardMenuAboutToShow()));

    connect(m_port, SIGNAL(serialPortListChanged()),
            this, SLOT(boardMenuAboutToShow()));

    connect(m_port, SIGNAL(portDestroyedOrChanged()),
            this, SLOT(boardMenuAboutToShow()));

    // Serial Escape Init /////////////////////////////////////////////////////

    m_escape = new SerialEscape(NULL, Core::ICore::mainWindow(),
                                Core::ICore::settings(), this);

    connect(m_escape, SIGNAL(errorMessage(QString)),
            this, SLOT(errorMessage(QString)));

    // Begin Registering //////////////////////////////////////////////////////

    qInstallMsgHandler(messageHandler);

    addAutoReleasedObject(new MicrocontrollerIDF);

    connect(static_cast<ProjectExplorer::DeviceManager *>
    (ProjectExplorer::DeviceManager::instance()),
    SIGNAL(devicesLoaded()), this, SLOT(deviceManagerSetup()));

    connect(static_cast<ProjectExplorer::ToolChainManager *>
    (ProjectExplorer::ToolChainManager::instance()),
    SIGNAL(toolChainsLoaded()), this, SLOT(toolchainManagerSetup()));

    connect(static_cast<QtSupport::QtVersionManager *>
    (QtSupport::QtVersionManager::instance()),
    SIGNAL(qtVersionsLoaded()), this, SLOT(versionsManagerSetup()));

    connect(static_cast<ProjectExplorer::KitManager *>
    (ProjectExplorer::KitManager::instance()),
    SIGNAL(kitsLoaded()), this, SLOT(kitManagerSetup()));

    cmakeManagerSetup();
    projectManagerSetup();
    environmentSetup();
    textEditorCppSetup();

    // New File or Project //
    {
        Core::ActionManager::command(
        Core::Constants::NEW)->action()->disconnect();

        connect(Core::ActionManager::command(
        Core::Constants::NEW)->action(),
        SIGNAL(triggered()), this,
        SLOT(newFileOrProject()));
    }

    // Open File or Project //
    {
        Core::ActionManager::command(
        Core::Constants::OPEN)->action()->disconnect();

        connect(Core::ActionManager::command(
        Core::Constants::OPEN)->action(),
        SIGNAL(triggered()), this,
        SLOT(openFileOrProject()));
    }

    // Build Buttons //
    {
        Core::ActionManager::command(
        ProjectExplorer::Constants::BUILDPROJECTONLY)->action()->disconnect();

        connect(Core::ActionManager::command(
        ProjectExplorer::Constants::BUILDPROJECTONLY)->action(),
        SIGNAL(triggered()), this,
        SLOT(buildClicked()));

        Core::ActionManager::command(
        ProjectExplorer::Constants::BUILD)->action()->disconnect();

        connect(Core::ActionManager::command(
        ProjectExplorer::Constants::BUILD)->action(),
        SIGNAL(triggered()), this,
        SLOT(buildClicked()));

        Core::ActionManager::command(
        ProjectExplorer::Constants::BUILDCM)->action()->disconnect();

        connect(Core::ActionManager::command(
        ProjectExplorer::Constants::BUILDCM)->action(),
        SIGNAL(triggered()), this,
        SLOT(buildClicked()));

        Core::ActionManager::command(
        ProjectExplorer::Constants::BUILDSESSION)->action()->disconnect();

        connect(Core::ActionManager::command(
        ProjectExplorer::Constants::BUILDSESSION)->action(),
        SIGNAL(triggered()), this,
        SLOT(buildClicked()));
    }

    // Run Buttons
    {
        Core::ActionManager::command(
        ProjectExplorer::Constants::RUN)->action()->disconnect();

        connect(Core::ActionManager::command(
        ProjectExplorer::Constants::RUN)->action(),
        SIGNAL(triggered()), this,
        SLOT(runClicked()));

        Core::ActionManager::command(
        ProjectExplorer::Constants::RUNCONTEXTMENU)->action()->disconnect();

        connect(Core::ActionManager::command(
        ProjectExplorer::Constants::RUNCONTEXTMENU)->action(),
        SIGNAL(triggered()), this,
        SLOT(runClicked()));

        Core::ActionManager::command(
        ProjectExplorer::Constants::RUNWITHOUTDEPLOY)->action()->disconnect();

        connect(Core::ActionManager::command(
        ProjectExplorer::Constants::RUNWITHOUTDEPLOY)->action(),
        SIGNAL(triggered()), this,
        SLOT(runClicked()));
    }

    Core::ActionManager::actionContainer(
    Core::Constants::M_FILE)->menu()->removeAction(
    Core::ActionManager::command(
    Core::Constants::OPEN_WITH)->action());

    Core::ActionManager::actionContainer(
    Core::Constants::M_WINDOW)->menu()->removeAction(
    Core::ActionManager::actionContainer(
    Core::Constants::M_WINDOW_VIEWS)->menu()->menuAction());

    // Board Menu //
    {
        m_boardMenu = Core::ActionManager::createMenu(
        BOARD_MENU_ID);

        m_boardMenu->setOnAllDisabledBehavior(
        Core::ActionContainer::Show);

        m_boardMenu->menu()->setTitle(
        tr("Board"));

        Core::ActionManager::actionContainer(
        Core::Constants::MENU_BAR)->addMenu(
        Core::ActionManager::actionContainer(
        Core::Constants::M_HELP), m_boardMenu);

        connect(m_boardMenu->menu(), SIGNAL(aboutToShow()),
                this, SLOT(boardMenuAboutToShow()));

        connect(m_boardMenu->menu(), SIGNAL(triggered(QAction*)),
                this, SLOT(boardMenuTriggered(QAction*)));
    }

    // Change Serial Port Board Name //
    {
        m_changeSerialPortBoardName = Core::ActionManager::registerAction(
        new QAction(tr("Change Serial Port Board Name"), m_boardMenu->menu()),
        CHANGE_SERIAL_PORT_BOARD_NAME_ACTION_ID,
        Core::Context(Core::Constants::C_GLOBAL));

        m_boardMenu->addAction(m_changeSerialPortBoardName);

        connect(m_changeSerialPortBoardName->action(), SIGNAL(triggered()),
                this, SLOT(changeSerialPortBoardName()));
    }

    // Change Serial Port Board Type //
    {
        m_changeSerialPortBoardType = Core::ActionManager::registerAction(
        new QAction(tr("Change Serial Port Board Type"), m_boardMenu->menu()),
        CHANGE_SERIAL_PORT_BOARD_TYPE_ACTION_ID,
        Core::Context(Core::Constants::C_GLOBAL));

        m_boardMenu->addAction(m_changeSerialPortBoardType);

        connect(m_changeSerialPortBoardType->action(), SIGNAL(triggered()),
                this, SLOT(changeSerialPortBoardType()));
    }

    m_boardMenu->menu()->addSeparator();

    // Serial Port Menu //
    {
        m_serialPortMenu = Core::ActionManager::createMenu(
        SERIAL_PORT_MENU_ID);

        m_serialPortMenu->setOnAllDisabledBehavior(
        Core::ActionContainer::Show);

        m_serialPortMenu->menu()->setTitle(
        tr("Serial Port"));

        m_boardMenu->addMenu(m_serialPortMenu);

        connect(m_serialPortMenu->menu(), SIGNAL(triggered(QAction*)),
                this, SLOT(serialPortSelected(QAction*)));

        // Add Status Bar Objects

        Core::ICore::statusBar()->insertPermanentWidget(labelIndex++,
        new QLabel(tr("Serial Port:"), Core::ICore::statusBar()));

        Core::ICore::statusBar()->insertPermanentWidget(labelIndex++,
        m_serialPortMenuSelection = new QLabel(Core::ICore::statusBar()));
    }

    Core::ICore::statusBar()->insertPermanentWidget(labelIndex++,
    new QLabel("|", Core::ICore::statusBar()));

    // Baud Rate Menu //
    {
        m_baudRateMenu = Core::ActionManager::createMenu(
        BAUD_RATE_MENU_ID);

        m_baudRateMenu->setOnAllDisabledBehavior(
        Core::ActionContainer::Show);

        m_baudRateMenu->menu()->setTitle(
        tr("Baud Rate"));

        m_boardMenu->addMenu(m_baudRateMenu);

        connect(m_baudRateMenu->menu(), SIGNAL(triggered(QAction*)),
                this, SLOT(baudRateSelected(QAction*)));

        // Add Status Bar Objects

        Core::ICore::statusBar()->insertPermanentWidget(labelIndex++,
        new QLabel(tr("Baud Rate:"), Core::ICore::statusBar()));

        Core::ICore::statusBar()->insertPermanentWidget(labelIndex++,
        m_baudRateMenuSelection = new QLabel(Core::ICore::statusBar()));
    }

    m_boardMenu->menu()->addSeparator();

    // Reset Serial Port //
    {
        m_resetSerialPort = Core::ActionManager::registerAction(
        new QAction(tr("Reset Serial Port"), this),
        RESET_SERIAL_PORT_ACTION_ID,
        Core::Context(Core::Constants::C_GLOBAL));

        m_boardMenu->addAction(m_resetSerialPort);

        connect(m_resetSerialPort->action(), SIGNAL(triggered()),
                m_escape, SLOT(userReset()));
    }

    // Widgets Menu //
    {
        m_widgetsMenu = Core::ActionManager::createMenu(
        WIDGETS_MENU_ID);

        m_widgetsMenu->setOnAllDisabledBehavior(
        Core::ActionContainer::Show);

        m_widgetsMenu->menu()->setTitle(
        tr("Widgets"));

        Core::ActionManager::actionContainer(
        Core::Constants::MENU_BAR)->addMenu(
        Core::ActionManager::actionContainer(
        Core::Constants::M_HELP), m_widgetsMenu);

        connect(m_widgetsMenu->menu(), SIGNAL(aboutToShow()),
                this, SLOT(widgetsMenuAboutToShow()));

        connect(m_widgetsMenu->menu(), SIGNAL(triggered(QAction*)),
                this, SLOT(widgetsMenuTriggered(QAction*)));
    }

    // Export Widget Menu //
    {
        m_exportWidgetMenu = Core::ActionManager::createMenu(
        EXPORT_WIDGET_STATE_MENU_ID);

        m_exportWidgetMenu->setOnAllDisabledBehavior(
        Core::ActionContainer::Show);

        m_exportWidgetMenu->menu()->setTitle(
        tr("Export Widget State"));

        m_widgetsMenu->addMenu(m_exportWidgetMenu);
    }

    // Import Widget //
    {
        m_importWidget = Core::ActionManager::registerAction(
        new QAction(tr("Import Widget State"), this),
        IMPORT_WIDGET_STATE_ACTION_ID,
        Core::Context(Core::Constants::C_GLOBAL));

        m_widgetsMenu->addAction(m_importWidget);

        connect(m_importWidget->action(), SIGNAL(triggered()),
                m_escape, SLOT(openJSON()));
    }

    m_widgetsMenu->menu()->addSeparator();

    // Remove Widget Menu //
    {
        m_removeWidgetMenu = Core::ActionManager::createMenu(
        REMOVE_WIDGET_MENU_ID);

        m_removeWidgetMenu->setOnAllDisabledBehavior(
        Core::ActionContainer::Show);

        m_removeWidgetMenu->menu()->setTitle(
        tr("Remove Widget"));

        m_widgetsMenu->addMenu(m_removeWidgetMenu);

        connect(m_removeWidgetMenu->menu(), SIGNAL(triggered(QAction*)),
                m_escape, SLOT(removeWidget(QAction*)));
    }

    // Remove All Widgets //
    {
        m_removeAllWidgets = Core::ActionManager::registerAction(
        new QAction(tr("Remove All Widgets"), this),
        REMOVE_ALL_WIDGETS_ACTION_ID,
        Core::Context(Core::Constants::C_GLOBAL));

        m_widgetsMenu->addAction(m_removeAllWidgets);

        connect(m_removeAllWidgets->action(), SIGNAL(triggered()),
                m_escape, SLOT(removeAllWidgets()));
    }

    m_widgetsMenu->menu()->addSeparator();

    // Reset Serial Terminal //
    {
        m_resetSerialTerminal = Core::ActionManager::registerAction(
        new QAction(tr("Reset Serial Terminal"), this),
        RESET_SERIAL_TERMINAL_ACTION_ID,
        Core::Context(Core::Constants::C_GLOBAL));

        m_widgetsMenu->addAction(m_resetSerialTerminal);

        connect(m_resetSerialTerminal->action(), SIGNAL(triggered()),
                m_escape->serialTerminal(), SLOT(userReset()));
    }

    Core::ActionManager::actionContainer(
    Core::Constants::M_HELP)->menu()->removeAction(
    Core::ActionManager::command(Core::Constants::ABOUT_QTCREATOR)->action());

    Core::ActionManager::actionContainer(
    Core::Constants::M_HELP)->menu()->removeAction(
    Core::ActionManager::command(Core::Constants::ABOUT_PLUGINS)->action());

    Core::ActionManager::actionContainer(
    Core::Constants::M_HELP)->menu()->clear();

    // General Help //
    {
        Core::Command *generalHelp = Core::ActionManager::registerAction(
        new QAction(tr("General Help"),
        Core::ActionManager::actionContainer(Core::Constants::M_HELP)->menu()),
        GENERAL_HELP_ID, Core::Context(Core::Constants::C_GLOBAL));

        Core::ActionManager::actionContainer(
        Core::Constants::M_HELP)->addAction(generalHelp);

        connect(generalHelp->action(), SIGNAL(triggered()),
                this, SLOT(generalHelp()));
    }

    // Editor Help //
    {
        Core::Command *editorHelp = Core::ActionManager::registerAction(
        new QAction(tr("Editor Help"),
        Core::ActionManager::actionContainer(Core::Constants::M_HELP)->menu()),
        EDITOR_HELP_ID, Core::Context(Core::Constants::C_GLOBAL));

        Core::ActionManager::actionContainer(
        Core::Constants::M_HELP)->addAction(editorHelp);

        connect(editorHelp->action(), SIGNAL(triggered()),
                this, SLOT(editorHelp()));
    }

    Core::ActionManager::actionContainer(
    Core::Constants::M_HELP)->menu()->addSeparator();

    // About //
    {
        Core::Command *about = Core::ActionManager::registerAction(
        new QAction(tr("About..."),
        Core::ActionManager::actionContainer(Core::Constants::M_HELP)->menu()),
        ABOUT_ACTION_ID, Core::Context(Core::Constants::C_GLOBAL));

        Core::ActionManager::actionContainer(
        Core::Constants::M_HELP)->addAction(about);

        connect(about->action(), SIGNAL(triggered()),
                this, SLOT(about()));
    }

    // About Qt //
    {
        Core::Command *aboutQt = Core::ActionManager::registerAction(
        new QAction(tr("About Qt..."),
        Core::ActionManager::actionContainer(Core::Constants::M_HELP)->menu()),
        ABOUT_QT_ACTION_ID, Core::Context(Core::Constants::C_GLOBAL));

        Core::ActionManager::actionContainer(
        Core::Constants::M_HELP)->addAction(aboutQt);

        connect(aboutQt->action(), SIGNAL(triggered()),
                QApplication::instance(), SLOT(aboutQt()));
    }

    Core::ActionManager::actionContainer(
    Core::Constants::M_HELP)->menu()->addSeparator();

    Core::ActionManager::actionContainer(
    Core::Constants::M_HELP)->menu()->addAction(
    Core::ActionManager::command(Core::Constants::ABOUT_QTCREATOR)->action());

    Core::ActionManager::actionContainer(
    Core::Constants::M_HELP)->menu()->addAction(
    Core::ActionManager::command(Core::Constants::ABOUT_PLUGINS)->action());

    // End Registering ////////////////////////////////////////////////////////

    boardMenuAboutToShow();
    widgetsMenuAboutToShow();

    ///////////////////////////////////////////////////////////////////////////

    return true;
}

void OmniaCreatorPlugin::extensionsInitialized()
{
    // Begin Action/Shortcut Workaround ///////////////////////////////////////

    QList<Core::Command *> list = Core::ActionManager::commands();

    foreach(Core::Command *command, list)
    {
        if(command->action())
        {
            command->action()->setShortcutContext(Qt::WindowShortcut);
        }

        if(command->shortcut())
        {
            command->shortcut()->setContext(Qt::WindowShortcut);
        }
    }

    // End Action/Shortcut Workaround /////////////////////////////////////////

    QString port = m_port->getLastPort();

    if(m_port->portInDemoPortList(port)
    || m_port->portInSerialPortList(port))
    {
        m_escape->setPort(m_port->openPort(port, true));
    }
}

bool OmniaCreatorPlugin::delayedInitialize()
{
    QSettings settings(Core::ICore::settings()->fileName(),
                       Core::ICore::settings()->format());

    settings.beginGroup(PLUGIN_DIALOG_KEY_GROUP);

    if(!settings.value(PLUGIN_DIALOG_KEY_NEW_SOFTWARE, false).toBool())
    {
        QMessageBox::information(Core::ICore::mainWindow(), tr("New Software"),
        tr("%L1 is new software. Please report any issues found at: "
        "<a href=\"https://github.com/omniacreator/omniacreator/issues\">"
        "%L1's GitHub</a>.").arg(QApplication::applicationName()));

        settings.setValue(PLUGIN_DIALOG_KEY_NEW_SOFTWARE, true);
    }

    settings.endGroup();

    ///////////////////////////////////////////////////////////////////////////

    if(!m_make->getWorkspaceFolderWasSet())
    {
        UtilPathPicker picker(tr("Select Workspace Location"),
        tr("Please select a workspace location"),
        m_make->getWorkspaceFolder(), Core::ICore::mainWindow());

        if(picker.exec() == QDialog::Accepted)
        {
            m_make->setWorkspaceFolder(picker.getDefaultDir());
            m_make->setWorkspaceFolderWasSet();
        }
    }

    ///////////////////////////////////////////////////////////////////////////

    if(!m_port->getLastPortWasSet())
    {
        QList< QPair<QString, QString> > list = m_port->getSerialPortList();

        int size = list.size();

        if(size > 1)
        {
            QString name = QApplication::applicationName();

            QMessageBox::StandardButton button =
            QMessageBox::question(Core::ICore::mainWindow(), tr("Select Port"),
            tr("%L1 found serial ports attached to your computer. "
            "Would you like to use one?").arg(name), QMessageBox::Yes |
            QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

            if(button == QMessageBox::Yes)
            {
                QList< QPair<QString, QVariant> > data;

                QList< QPair<QString, QString> >::const_iterator i =
                list.constBegin();

                QList< QPair<QString, QString> >::const_iterator j =
                list.constEnd();

                for(; i != j; i++)
                {
                    data.append(QPair<QString, QVariant>(i->first, i->second));
                }

                UtilItemPicker ip(tr("Select Port"),
                tr("Please select a serial port"),
                data, Core::ICore::mainWindow());

                if(ip.exec() == QDialog::Accepted)
                {
                    // Don't ask again...

                    m_port->setLastPortWasSet();

                    m_escape->setPort(m_port->openPort(
                    ip.getSelectedItem().toString(), false));
                }
            }
            else if(button == QMessageBox::No)
            {
                QMessageBox::information(Core::ICore::mainWindow(),
                tr("Select Port"), tr("Please see the <b>Board Menu</b> "
                "to select a serial port in the future"), QMessageBox::Ok,
                QMessageBox::Ok);

                // Don't ask again...

                m_port->setLastPortWasSet();
            }
        }
        else if(size > 0)
        {
            QString name = QApplication::applicationName();

            QMessageBox::StandardButton button =
            QMessageBox::question(Core::ICore::mainWindow(), tr("Select Port"),
            tr("%L1 found a serial port attached to your computer. "
            "Would you like to use it?").arg(name), QMessageBox::Yes |
            QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

            if(button == QMessageBox::Yes)
            {
                QList< QPair<QString, QVariant> > data;

                QList< QPair<QString, QString> >::const_iterator i =
                list.constBegin();

                QList< QPair<QString, QString> >::const_iterator j =
                list.constEnd();

                for(; i != j; i++)
                {
                    data.append(QPair<QString, QVariant>(i->first, i->second));
                }

                UtilItemPicker ip(tr("Select Port"),
                tr("Please select the serial port"),
                data, Core::ICore::mainWindow());

                if(ip.exec() == QDialog::Accepted)
                {
                    // Don't ask again...

                    m_port->setLastPortWasSet();

                    m_escape->setPort(m_port->openPort(
                    ip.getSelectedItem().toString(), false));
                }
            }
            else if(button == QMessageBox::No)
            {
                QMessageBox::information(Core::ICore::mainWindow(),
                tr("Select Port"), tr("Please see the <b>Board Menu</b> "
                "to select a serial port in the future"), QMessageBox::Ok,
                QMessageBox::Ok);

                // Don't ask again...

                m_port->setLastPortWasSet();
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////

    QProcess *update = new QProcess(this);

    connect(update, SIGNAL(finished(int)), this, SLOT(processMessage(int)));
    connect(update, SIGNAL(finished(int)), update, SLOT(deleteLater()));

#if defined(Q_OS_WIN)
    update->start(QDir::fromNativeSeparators(QDir::cleanPath(
    QApplication::applicationDirPath() + "/update.exe")),
#else
    update->start(QDir::fromNativeSeparators(QDir::cleanPath(
    QApplication::applicationDirPath() + "/update")),
#endif

    QStringList() << "--mode" << "unattended");

    ///////////////////////////////////////////////////////////////////////////

    return true;
}

ExtensionSystem::IPlugin::ShutdownFlag OmniaCreatorPlugin::aboutToShutdown()
{
    delete m_make;
    delete m_port;
    delete m_escape;

    SerialOscilloscope::finiFftw();

    closeProject();

    return ExtensionSystem::IPlugin::SynchronousShutdown;
}

void OmniaCreatorPlugin::newFileOrProject()
{
    QSettings settings(Core::ICore::settings()->fileName(),
                       Core::ICore::settings()->format());

    settings.beginGroup(PLUGIN_DIALOG_KEY_GROUP);

    NewFileOrProjectDialog dialog1(tr("New..."),
    Core::ICore::mainWindow());

    switch(dialog1.exec())
    {
        case NewFileOrProjectDialog::newFileWasPressed:
        {
            QString openPath = settings.value(PLUGIN_DIALOG_KEY_NEW_FILE,
            QDir::fromNativeSeparators(QDir::homePath() +
            QDir::separator() + tr("Untitled.cpp"))).toString();

            NewFileDialog dialog2(tr("New File"),
            QFileInfo(openPath).fileName(), QFileInfo(openPath).path(),
            Core::ICore::mainWindow());

            if(dialog2.exec() == QDialog::Accepted)
            {
                QString fullPath =
                QDir::fromNativeSeparators(dialog2.getDefaultDir() +
                QDir::separator() + dialog2.getName());

                if(!QDir(dialog2.getDefaultDir()).exists())
                {
                    if(!QDir().mkpath(dialog2.getDefaultDir()))
                    {
                        QMessageBox::critical(Core::ICore::mainWindow(),
                        dialog2.windowTitle(), tr("Unable to create path!"));
                        break;
                    }
                }

                QString time = QDateTime::currentDateTime().toString();

                if(fullPath.endsWith(".c")
                || fullPath.endsWith(".i")
                || fullPath.endsWith(".cogc")) // For Propeller
                {
                    QFile file(fullPath);

                    if(file.open(QIODevice::WriteOnly))
                    {
                        QByteArray text = QString(
                        "/**\n"
                        "* @file %L1\n"
                        "* Brief Description\n"
                        "*\n"
                        "* @version @n 0.1\n"
                        "* @date @n %L2\n"
                        "*\n"
                        "* @author @n %L3\n"
                        "*/\n"
                        "\n\n"
                        ).arg(QFileInfo(file).fileName()).arg(time).
                        arg(QString(qgetenv(BY_NAME))).toUtf8();

                        if(file.write(text) != text.size())
                        {
                            QMessageBox::critical(Core::ICore::mainWindow(),
                            dialog2.windowTitle(), file.errorString());
                            break;
                        }

                        file.close();
                    }
                    else
                    {
                        QMessageBox::critical(Core::ICore::mainWindow(),
                        dialog2.windowTitle(), file.errorString());
                        break;
                    }
                }
                else if(fullPath.endsWith(".cpp")
                || fullPath.endsWith(".ii")
                || fullPath.endsWith(".cc")
                || fullPath.endsWith(".cp")
                || fullPath.endsWith(".cxx")
                || fullPath.endsWith(".c++")
                || fullPath.endsWith(".cogcpp")) // For Propeller
                {
                    QFile file(fullPath);

                    if(file.open(QIODevice::WriteOnly))
                    {
                        QByteArray text = QString(
                        "/**\n"
                        "* @file %L1\n"
                        "* Brief Description\n"
                        "*\n"
                        "* @version @n 0.1\n"
                        "* @date @n %L2\n"
                        "*\n"
                        "* @author @n %L3\n"
                        "*/\n"
                        "\n\n"
                        ).arg(QFileInfo(file).fileName()).arg(time).
                        arg(QString(qgetenv(BY_NAME))).toUtf8();

                        if(file.write(text) != text.size())
                        {
                            QMessageBox::critical(Core::ICore::mainWindow(),
                            dialog2.windowTitle(), file.errorString());
                            break;
                        }

                        file.close();
                    }
                    else
                    {
                        QMessageBox::critical(Core::ICore::mainWindow(),
                        dialog2.windowTitle(), file.errorString());
                        break;
                    }
                }
                else if(fullPath.endsWith(".h")
                || fullPath.endsWith(".hpp")
                || fullPath.endsWith(".hh")
                || fullPath.endsWith(".hp")
                || fullPath.endsWith(".hxx")
                || fullPath.endsWith(".h++"))
                {
                    QFile file(fullPath);

                    if(file.open(QIODevice::WriteOnly))
                    {
                        QByteArray text = QString(
                        "/**\n"
                        "* @file %L1\n"
                        "* Brief Description\n"
                        "*\n"
                        "* @version @n 0.1\n"
                        "* @date @n %L2\n"
                        "*\n"
                        "* @author @n %L3\n"
                        "*/\n"
                        "\n"
                        "#ifndef %L4\n"
                        "#define %L4\n"
                        "\n\n\n"
                        "#endif // %L4\n"
                        ).arg(QFileInfo(file).fileName()).arg(time).
                        arg(QString(qgetenv(BY_NAME))).
                        arg(QFileInfo(file).fileName().toUpper().
                        replace(QRegularExpression("[^0-9A-Za-z]"),
                        "_")).toUtf8();

                        if(file.write(text) != text.size())
                        {
                            QMessageBox::critical(Core::ICore::mainWindow(),
                            dialog2.windowTitle(), file.errorString());
                            break;
                        }

                        file.close();
                    }
                    else
                    {
                        QMessageBox::critical(Core::ICore::mainWindow(),
                        dialog2.windowTitle(), file.errorString());
                        break;
                    }
                }
                else if(fullPath.endsWith(".ino") // For Arduino
                || fullPath.endsWith(".pde")) // For Arduino
                {
                    QFile file(fullPath);

                    if(file.open(QIODevice::WriteOnly))
                    {
                        QByteArray text = QString(
                        "/**\n"
                        "* @file %L1\n"
                        "* Brief Description\n"
                        "*\n"
                        "* @version @n 0.1\n"
                        "* @date @n %L2\n"
                        "*\n"
                        "* @author @n %L3\n"
                        "*/\n"
                        "\n"
                        "void setup()\n"
                        "{\n"
                        "\n"
                        "}\n"
                        "\n"
                        "void loop()\n"
                        "{\n"
                        "\n"
                        "}\n"
                        ).arg(QFileInfo(file).fileName()).arg(time).
                        arg(QString(qgetenv(BY_NAME))).toUtf8();

                        if(file.write(text) != text.size())
                        {
                            QMessageBox::critical(Core::ICore::mainWindow(),
                            dialog2.windowTitle(), file.errorString());
                            break;
                        }

                        file.close();
                    }
                    else
                    {
                        QMessageBox::critical(Core::ICore::mainWindow(),
                        dialog2.windowTitle(), file.errorString());
                        break;
                    }
                }
                else if(fullPath.endsWith(".s")
                || fullPath.endsWith(".sx"))
                {
                    QFile file(fullPath);

                    if(file.open(QIODevice::WriteOnly))
                    {
                        QByteArray text = QString(
                        "/**\n"
                        "* @file %L1\n"
                        "* Brief Description\n"
                        "* \n"
                        "* @version @n 0.1\n"
                        "* @date @n %L2\n"
                        "* \n"
                        "* @author @n %L3\n"
                        "*/\n"
                        "\n\n"
                        ).arg(QFileInfo(file).fileName()).arg(time).
                        arg(QString(qgetenv(BY_NAME))).toUtf8();

                        if(file.write(text) != text.size())
                        {
                            QMessageBox::critical(Core::ICore::mainWindow(),
                            dialog2.windowTitle(), file.errorString());
                            break;
                        }

                        file.close();
                    }
                    else
                    {
                        QMessageBox::critical(Core::ICore::mainWindow(),
                        dialog2.windowTitle(), file.errorString());
                        break;
                    }
                }
                else if(fullPath.endsWith(".spin") // For Propeller
                || fullPath.endsWith(".spin2")) // For Propeller
                {
                    QFile file(fullPath);

                    if(file.open(QIODevice::WriteOnly))
                    {
                        QByteArray text = QString(
                        "{{\n"
                        "@file %L1\n"
                        "Brief Description\n"
                        "\n"
                        "@version @n 0.1\n"
                        "@date @n %L2\n"
                        "\n"
                        "@author @n %L3\n"
                        "}}\n"
                        "\n\n"
                        ).arg(QFileInfo(file).fileName()).arg(time).
                        arg(QString(qgetenv(BY_NAME))).toUtf8();

                        if(file.write(text) != text.size())
                        {
                            QMessageBox::critical(Core::ICore::mainWindow(),
                            dialog2.windowTitle(), file.errorString());
                            break;
                        }

                        file.close();
                    }
                    else
                    {
                        QMessageBox::critical(Core::ICore::mainWindow(),
                        dialog2.windowTitle(), file.errorString());
                        break;
                    }
                }
                else
                {
                    QFile file(fullPath);

                    if(file.open(QIODevice::WriteOnly))
                    {
                        QByteArray text = QString().toUtf8();

                        if(file.write(text) != text.size())
                        {
                            QMessageBox::critical(Core::ICore::mainWindow(),
                            dialog2.windowTitle(), file.errorString());
                            break;
                        }

                        file.close();
                    }
                    else
                    {
                        QMessageBox::critical(Core::ICore::mainWindow(),
                        dialog2.windowTitle(), file.errorString());
                        break;
                    }
                }

                Core::EditorManager::openEditor(fullPath);

                QFileInfo fileInfo(fullPath);

                settings.setValue(PLUGIN_DIALOG_KEY_NEW_FILE,
                fileInfo.canonicalFilePath());
            }

            break;
        }

        case NewFileOrProjectDialog::newProjectWasPressed:
        {
            QString openPath = settings.value(PLUGIN_DIALOG_KEY_NEW_PROJECT,
            QDir::fromNativeSeparators(QDir::homePath() +
            QDir::separator() + tr("Untitled"))).toString();

            NewProjectDialog dialog2(tr("New Project"),
            QFileInfo(openPath).fileName(), QFileInfo(openPath).path(),
            Core::ICore::mainWindow());

            if(dialog2.exec() == QDialog::Accepted)
            {
                QString fullPath =
                QDir::fromNativeSeparators(dialog2.getDefaultDir() +
                QDir::separator() + dialog2.getName());

                if(!QDir(fullPath).exists())
                {
                    if(!QDir().mkpath(fullPath))
                    {
                        QMessageBox::critical(Core::ICore::mainWindow(),
                        dialog2.windowTitle(), tr("Unable to create path!"));
                        break;
                    }
                }

                QString time = QDateTime::currentDateTime().toString();

                if(dialog2.getCreateMainFiles())
                {
                    if(dialog2.getCreateCMainFile())
                    {
                        // Make H File
                        {
                            QFile file(QDir::fromNativeSeparators(
                            fullPath + QDir::separator() +
                            dialog2.getName() + ".h"));

                            if(file.open(QIODevice::WriteOnly))
                            {
                                QByteArray text = QString(
                                "/**\n"
                                "* @file %L1\n"
                                "* Brief Description\n"
                                "*\n"
                                "* @version @n 0.1\n"
                                "* @date @n %L2\n"
                                "*\n"
                                "* @author @n %L3\n"
                                "*/\n"
                                "\n"
                                "#ifndef %L4\n"
                                "#define %L4\n"
                                "\n"
                                "#include <InterfaceLibrary.h>\n"
                                "\n"
                                "int main();\n"
                                "\n"
                                "#endif // %L4\n"
                                ).arg(QFileInfo(file).fileName()).arg(time).
                                arg(QString(qgetenv(BY_NAME))).
                                arg(QFileInfo(file).fileName().toUpper().
                                replace(QRegularExpression("[^0-9A-Za-z]"),
                                "_")).toUtf8();

                                if(file.write(text) != text.size())
                                {
                                    QMessageBox::critical(
                                    Core::ICore::mainWindow(),
                                    dialog2.windowTitle(),
                                    file.errorString());
                                    break;
                                }

                                file.close();
                            }
                            else
                            {
                                QMessageBox::critical(
                                Core::ICore::mainWindow(),
                                dialog2.windowTitle(),
                                file.errorString());
                                break;
                            }

                            Core::EditorManager::openEditor(file.fileName());
                        }

                        // Make C File
                        {
                            QFile file(QDir::fromNativeSeparators(
                            fullPath + QDir::separator() +
                            dialog2.getName() + ".c"));

                            if(file.open(QIODevice::WriteOnly))
                            {
                                QByteArray text = QString(
                                "/**\n"
                                "* @file %L1\n"
                                "* Brief Description\n"
                                "*\n"
                                "* @version @n 0.1\n"
                                "* @date @n %L2\n"
                                "*\n"
                                "* @author @n %L3\n"
                                "*/\n"
                                "\n"
                                "#include \"%L4\"\n"
                                "\n"
                                "int main()\n"
                                "{\n"
                                "\n"
                                "}\n"
                                ).arg(QFileInfo(file).fileName()).arg(time).
                                arg(QString(qgetenv(BY_NAME))).
                                arg(dialog2.getName() + ".h").toUtf8();

                                if(file.write(text) != text.size())
                                {
                                    QMessageBox::critical(
                                    Core::ICore::mainWindow(),
                                    dialog2.windowTitle(),
                                    file.errorString());
                                    break;
                                }

                                file.close();
                            }
                            else
                            {
                                QMessageBox::critical(
                                Core::ICore::mainWindow(),
                                dialog2.windowTitle(),
                                file.errorString());
                                break;
                            }

                            Core::EditorManager::openEditor(file.fileName());
                        }
                    }
                    else if(dialog2.getCreateCppMainFile())
                    {
                        // Make H File
                        {
                            QFile file(QDir::fromNativeSeparators(
                            fullPath + QDir::separator() +
                            dialog2.getName() + ".h"));

                            if(file.open(QIODevice::WriteOnly))
                            {
                                QByteArray text = QString(
                                "/**\n"
                                "* @file %L1\n"
                                "* Brief Description\n"
                                "*\n"
                                "* @version @n 0.1\n"
                                "* @date @n %L2\n"
                                "*\n"
                                "* @author @n %L3\n"
                                "*/\n"
                                "\n"
                                "#ifndef %L4\n"
                                "#define %L4\n"
                                "\n"
                                "#include <InterfaceLibrary.h>\n"
                                "\n"
                                "int main();\n"
                                "\n"
                                "#endif // %L4\n"
                                ).arg(QFileInfo(file).fileName()).arg(time).
                                arg(QString(qgetenv(BY_NAME))).
                                arg(QFileInfo(file).fileName().toUpper().
                                replace(QRegularExpression("[^0-9A-Za-z]"),
                                "_")).toUtf8();

                                if(file.write(text) != text.size())
                                {
                                    QMessageBox::critical(
                                    Core::ICore::mainWindow(),
                                    dialog2.windowTitle(),
                                    file.errorString());
                                    break;
                                }

                                file.close();
                            }
                            else
                            {
                                QMessageBox::critical(
                                Core::ICore::mainWindow(),
                                dialog2.windowTitle(),
                                file.errorString());
                                break;
                            }

                            Core::EditorManager::openEditor(file.fileName());
                        }

                        // Make CPP File
                        {
                            QFile file(QDir::fromNativeSeparators(
                            fullPath + QDir::separator() +
                            dialog2.getName() + ".cpp"));

                            if(file.open(QIODevice::WriteOnly))
                            {
                                QByteArray text = QString(
                                "/**\n"
                                "* @file %L1\n"
                                "* Brief Description\n"
                                "*\n"
                                "* @version @n 0.1\n"
                                "* @date @n %L2\n"
                                "*\n"
                                "* @author @n %L3\n"
                                "*/\n"
                                "\n"
                                "#include \"%L4\"\n"
                                "\n"
                                "int main()\n"
                                "{\n"
                                "\n"
                                "}\n"
                                ).arg(QFileInfo(file).fileName()).arg(time).
                                arg(QString(qgetenv(BY_NAME))).
                                arg(dialog2.getName() + ".h").toUtf8();

                                if(file.write(text) != text.size())
                                {
                                    QMessageBox::critical(
                                    Core::ICore::mainWindow(),
                                    dialog2.windowTitle(),
                                    file.errorString());
                                    break;
                                }

                                file.close();
                            }
                            else
                            {
                                QMessageBox::critical(
                                Core::ICore::mainWindow(),
                                dialog2.windowTitle(),
                                file.errorString());
                                break;
                            }

                            Core::EditorManager::openEditor(file.fileName());
                        }
                    }
                    else if(dialog2.getCreateArduinoMainFile())
                    {
                        QFile file(QDir::fromNativeSeparators(
                        fullPath + QDir::separator() +
                        dialog2.getName() + ".ino"));

                        if(file.open(QIODevice::WriteOnly))
                        {
                            QByteArray text = QString(
                            "/**\n"
                            "* @file %L1\n"
                            "* Brief Description\n"
                            "*\n"
                            "* @version @n 0.1\n"
                            "* @date @n %L2\n"
                            "*\n"
                            "* @author @n %L3\n"
                            "*/\n"
                            "\n"
                            "#include <InterfaceLibrary.h>\n"
                            "\n"
                            "void setup()\n"
                            "{\n"
                            "\n"
                            "}\n"
                            "\n"
                            "void loop()\n"
                            "{\n"
                            "\n"
                            "}\n"
                            ).arg(QFileInfo(file).fileName()).arg(time).
                            arg(QString(qgetenv(BY_NAME))).toUtf8();

                            if(file.write(text) != text.size())
                            {
                                QMessageBox::critical(
                                Core::ICore::mainWindow(),
                                dialog2.windowTitle(),
                                file.errorString());
                                break;
                            }

                            file.close();
                        }
                        else
                        {
                            QMessageBox::critical(
                            Core::ICore::mainWindow(),
                            dialog2.windowTitle(),
                            file.errorString());
                            break;
                        }

                        Core::EditorManager::openEditor(file.fileName());
                    }
                    else
                    {
                        Q_UNREACHABLE();
                    }
                }

                m_make->setProjectFolder(fullPath);

                QFileInfo fileInfo(fullPath);

                settings.setValue(PLUGIN_DIALOG_KEY_NEW_PROJECT,
                fileInfo.canonicalFilePath());
            }

            break;
        }

        default: break;
    }
}

void OmniaCreatorPlugin::openFileOrProject()
{
    QSettings settings(Core::ICore::settings()->fileName(),
                       Core::ICore::settings()->format());

    settings.beginGroup(PLUGIN_DIALOG_KEY_GROUP);

    OpenFileOrProjectDialog dialog1(tr("Open..."),
    Core::ICore::mainWindow());

    switch(dialog1.exec())
    {
        case OpenFileOrProjectDialog::openFileWasPressed:
        {
            QString openPath = settings.value(PLUGIN_DIALOG_KEY_OPEN_FILE,
                                              QDir::homePath()).toString();

            QStringList temp =
            QFileDialog::getOpenFileNames(Core::ICore::mainWindow(),
            tr("Open File"), openPath, tr("All Files (*)"));

            foreach(const QString &string, temp)
            {
                if(!string.isEmpty())
                {
                    Core::EditorManager::openEditor(string);

                    QFileInfo fileInfo(string);

                    settings.setValue(PLUGIN_DIALOG_KEY_OPEN_FILE,
                    fileInfo.canonicalPath());
                }
            }

            break;
        }

        case OpenFileOrProjectDialog::openProjectWasPressed:
        {
            QString openPath = settings.value(PLUGIN_DIALOG_KEY_OPEN_PROJECT,
                                              QDir::homePath()).toString();

            QString temp =
            QFileDialog::getExistingDirectory(Core::ICore::mainWindow(),
            tr("Open Project"), openPath);

            if(!temp.isEmpty())
            {
                foreach(const QString &fp,
                QDir(temp).entryList(QStringList() <<
                '*' + QFileInfo(temp).completeBaseName() + '*',
                QDir::Files | QDir::NoDotAndDotDot | QDir::CaseSensitive,
                QDir::Name | QDir::Reversed | QDir::LocaleAware))
                {
                    Core::EditorManager::openEditor(QDir(temp).filePath(fp));
                }

                m_make->setProjectFolder(temp);

                QFileInfo fileInfo(temp);

                settings.setValue(PLUGIN_DIALOG_KEY_OPEN_PROJECT,
                fileInfo.canonicalPath());
            }

            break;
        }

        default: break;
    }
}

void OmniaCreatorPlugin::boardMenuAboutToShow()
{
    bool portOpen = m_port->getPort(); bool demoPort = m_port->isDemoPort();

    m_changeSerialPortBoardName->action()->setEnabled(portOpen && (!demoPort));
    m_changeSerialPortBoardType->action()->setEnabled(!demoPort);
    m_resetSerialPort->action()->setEnabled(portOpen && (!demoPort));

    updateSerialPortMenu();
    updateBaudRateMenu();
}

void OmniaCreatorPlugin::widgetsMenuAboutToShow()
{
    m_widgetsMenu->menu()->clear();

    {
        SerialWindow *window = m_escape->serialTerminal();

        QAction *action = new QAction(window->windowTitle().
        replace('&', "&&"), m_widgetsMenu->menu());

        connect(window, SIGNAL(destroyed()),
                action, SLOT(deleteLater()));

        action->setCheckable(true);
        action->setChecked(window->isVisible());
        action->setEnabled(true);
        action->setData(QVariant::fromValue(window));

        m_widgetsMenu->menu()->addAction(action);
    }

    m_widgetsMenu->menu()->addSeparator();

    {
        foreach(SerialWindow *window, m_escape->serialWindows())
        {
            QAction *action = new QAction(window->windowTitle().
            replace('&', "&&"),  m_widgetsMenu->menu());

            connect(window, SIGNAL(destroyed()),
                    action, SLOT(deleteLater()));

            action->setCheckable(true);
            action->setChecked(window->isVisible());
            action->setEnabled(true);
            action->setData(QVariant::fromValue(window));

            m_widgetsMenu->menu()->addAction(action);
        }
    }

    m_widgetsMenu->menu()->addSeparator();

    m_widgetsMenu->menu()->addMenu(m_exportWidgetMenu->menu());
    m_widgetsMenu->menu()->addAction(m_importWidget->action());

    m_widgetsMenu->menu()->addSeparator();

    m_widgetsMenu->menu()->addMenu(m_removeWidgetMenu->menu());
    m_widgetsMenu->menu()->addAction(m_removeAllWidgets->action());

    m_removeAllWidgets->action()->setDisabled(
    m_escape->serialWindows().isEmpty());

    m_widgetsMenu->menu()->addSeparator();

    m_widgetsMenu->menu()->addAction(m_resetSerialTerminal->action());

    updateExportWidgetMenu();
    updateRemoveWidgetMenu();
}

void OmniaCreatorPlugin::boardMenuTriggered(QAction *action)
{
    Q_UNUSED(action);
}

void OmniaCreatorPlugin::widgetsMenuTriggered(QAction *action)
{
    if(action && (action->parent() == m_widgetsMenu->menu()))
    {
        SerialWindow *window = qvariant_cast<SerialWindow *>(action->data());

        if((m_escape->serialTerminal() == window)
        || m_escape->serialWindows().contains(window))
        {
            window->setVisible(!window->isVisible());
        }
    }
}

void OmniaCreatorPlugin::changeSerialPortBoardName()
{
    QString name = m_port->getPortName();

    if(!name.isEmpty())
    {
        m_port->setPrettyNameWithDialog(name);
    }
}

void OmniaCreatorPlugin::changeSerialPortBoardType()
{
    QString name = m_port->getPortName();

    if(!name.isEmpty())
    {
        m_port->setMakeFileWithDialog(name);
    }
    else
    {
        m_make->setMakeFileWithDialog();
    }
}

void OmniaCreatorPlugin::updateSerialPortMenu()
{
    m_serialPortMenu->menu()->clear();

    {
        QAction *action = new QAction(tr("None").
        replace('&', "&&"), m_serialPortMenu->menu());

        bool isMe = m_port->getPortName().isEmpty();

        action->setCheckable(isMe);
        action->setChecked(isMe);
        action->setDisabled(isMe);
        action->setData(QString());

        m_serialPortMenu->menu()->addAction(action);

        if(isMe)
        {
            m_serialPortMenuSelection->setText(tr("None"));
        }
    }

    m_serialPortMenu->menu()->addSeparator();

    {
        QList< QPair<QString, QString> > serialPortList =
        m_port->getSerialPortList();

        QList< QPair<QString, QString> >::const_iterator i =
        serialPortList.constBegin();

        QList< QPair<QString, QString> >::const_iterator j =
        serialPortList.constEnd();

        for(; i != j; i++)
        {
            QAction *action = new QAction(QString(i->first).
            replace('&', "&&"), m_serialPortMenu->menu());

            bool isMe = m_port->getPortName() == i->second;

            action->setCheckable(isMe);
            action->setChecked(isMe);
            action->setDisabled(isMe);
            action->setData(i->second);

            m_serialPortMenu->menu()->addAction(action);

            if(isMe)
            {
                m_serialPortMenuSelection->setText(i->first);
            }
        }
    }

    m_serialPortMenu->menu()->addSeparator();

    {
        QList< QPair<QString, QString> > demoPortList =
        m_port->getDemoPortList();

        QList< QPair<QString, QString> >::const_iterator i =
        demoPortList.constBegin();

        QList< QPair<QString, QString> >::const_iterator j =
        demoPortList.constEnd();

        for(; i != j; i++)
        {
            QAction *action = new QAction(QString(i->first).
            replace('&', "&&"), m_serialPortMenu->menu());

            bool isMe = m_port->getPortName() == i->second;

            action->setCheckable(isMe);
            action->setChecked(isMe);
            action->setDisabled(isMe);
            action->setData(i->second);

            m_serialPortMenu->menu()->addAction(action);

            if(isMe)
            {
                m_serialPortMenuSelection->setText(i->first);
            }
        }
    }
}

void OmniaCreatorPlugin::serialPortSelected(QAction *action)
{
    if(action && (action->parent() == m_serialPortMenu->menu()))
    {
        QString string = action->data().toString();

        m_escape->setPort(m_port->openPort(string, false));
    }
}

void OmniaCreatorPlugin::updateBaudRateMenu()
{
    m_baudRateMenu->menu()->clear();

    {
        QAction *action = new QAction(tr("Auto").
        replace('&', "&&"), m_baudRateMenu->menu());

        QString portName = m_port->getPortName();
        bool isMe = m_port->getOverrideBaudRate(portName).isEmpty();

        action->setCheckable(isMe);
        action->setChecked(isMe);
        action->setDisabled(isMe
        || portName.isEmpty()
        || m_port->isDemoPort());
        action->setData(QString());

        m_baudRateMenu->menu()->addAction(action);

        if(isMe)
        {
            m_baudRateMenuSelection->setText(tr("Auto"));
        }
    }

    m_baudRateMenu->menu()->addSeparator();

    {
        QList< QPair<QString, QString> > list;

        list.append(QPair<QString, QString>("110", "110"));
        list.append(QPair<QString, QString>("300", "300"));
        list.append(QPair<QString, QString>("600", "600"));
        list.append(QPair<QString, QString>("1200", "1200"));
        list.append(QPair<QString, QString>("2400", "2400"));
        list.append(QPair<QString, QString>("4800", "4800"));
        list.append(QPair<QString, QString>("9600", "9600"));
        list.append(QPair<QString, QString>("14400", "14400"));
        list.append(QPair<QString, QString>("19200", "19200"));
        list.append(QPair<QString, QString>("28800", "28800"));
        list.append(QPair<QString, QString>("31250", "31250"));
        list.append(QPair<QString, QString>("38400", "38400"));
        list.append(QPair<QString, QString>("57600", "57600"));
        list.append(QPair<QString, QString>("115200", "115200"));
        list.append(QPair<QString, QString>("230400", "230400"));
        list.append(QPair<QString, QString>("250000", "250000"));
        list.append(QPair<QString, QString>("460800", "460800"));
        list.append(QPair<QString, QString>("921600", "921600"));
        list.append(QPair<QString, QString>("1000000", "1000000"));
        list.append(QPair<QString, QString>("1843200", "1843200"));
        list.append(QPair<QString, QString>("2000000", "2000000"));
        list.append(QPair<QString, QString>("3000000", "3000000"));

        QList< QPair<QString, QString> >::const_iterator i =
        list.constBegin();

        QList< QPair<QString, QString> >::const_iterator j =
        list.constEnd();

        for(; i != j; i++)
        {
            QAction *action = new QAction(QString(i->first).
            replace('&', "&&"), m_baudRateMenu->menu());

            QString portName = m_port->getPortName();
            bool isMe = m_port->getOverrideBaudRate(portName) == i->second;

            action->setCheckable(isMe);
            action->setChecked(isMe);
            action->setDisabled(isMe
            || portName.isEmpty()
            || m_port->isDemoPort());
            action->setData(i->second);

            m_baudRateMenu->menu()->addAction(action);

            if(isMe)
            {
                m_baudRateMenuSelection->setText(i->first);
            }
        }
    }
}

void OmniaCreatorPlugin::baudRateSelected(QAction *action)
{
    if(action && (action->parent() == m_baudRateMenu->menu()))
    {
        QString string = action->data().toString();

        m_port->setOverrideBaudRate(m_port->getPortName(), string);
    }
}

void OmniaCreatorPlugin::updateExportWidgetMenu()
{
    m_exportWidgetMenu->menu()->clear();

    {
        SerialWindow *window = m_escape->serialTerminal();

        if(window->importExportEnabled())
        {
            QAction *action = new QAction(window->windowTitle().
            replace('&', "&&"), m_exportWidgetMenu->menu());

            connect(window, SIGNAL(destroyed()),
                    action, SLOT(deleteLater()));

            connect(action, SIGNAL(triggered()),
                    window, SLOT(exportState()));

            m_exportWidgetMenu->menu()->addAction(action);
        }
    }

    m_exportWidgetMenu->menu()->addSeparator();

    {
        foreach(SerialWindow *window, m_escape->serialWindows())
        {
            if(window->importExportEnabled())
            {
                QAction *action = new QAction(window->windowTitle().
                replace('&', "&&"), m_exportWidgetMenu->menu());

                connect(window, SIGNAL(destroyed()),
                        action, SLOT(deleteLater()));

                connect(action, SIGNAL(triggered()),
                        window, SLOT(exportState()));

                m_exportWidgetMenu->menu()->addAction(action);
            }
        }
    }

    m_exportWidgetMenu->menu()->setDisabled(
    m_exportWidgetMenu->menu()->isEmpty());
}

void OmniaCreatorPlugin::updateRemoveWidgetMenu()
{
    m_removeWidgetMenu->menu()->clear();

    {
        foreach(SerialWindow *window, m_escape->serialWindows())
        {
            QAction *action = new QAction(window->windowTitle().
            replace('&', "&&"), m_removeWidgetMenu->menu());

            connect(window, SIGNAL(destroyed()),
                    action, SLOT(deleteLater()));

            action->setCheckable(false);
            action->setChecked(false);
            action->setEnabled(true);
            action->setData(QVariant::fromValue(window));

            m_removeWidgetMenu->menu()->addAction(action);
        }
    }

    m_removeWidgetMenu->menu()->setDisabled(
    m_removeWidgetMenu->menu()->isEmpty());
}

void OmniaCreatorPlugin::generalHelp()
{
    if(!QDesktopServices::openUrl(QUrl("http://" PROJECT_DOMAIN_NAME_STR "/"
    "help/general/")))
    {
        QMessageBox::critical(Core::ICore::mainWindow(),
        tr("Open General Help Error"),
        tr("Unable to open the URL to the General Help page"));
    }
}

void OmniaCreatorPlugin::editorHelp()
{
    if(!QDesktopServices::openUrl(QUrl("http://" PROJECT_DOMAIN_NAME_STR "/"
    "help/editor/")))
    {
        QMessageBox::critical(Core::ICore::mainWindow(),
        tr("Open Editor Help Error"),
        tr("Unable to open the URL to the Editor Help page"));
    }
}

void OmniaCreatorPlugin::about()
{
    QMessageBox::about(Core::ICore::mainWindow(), tr("About %L1 %L2").
    arg(QApplication::applicationName()).
    arg(QApplication::applicationVersion()),
    tr("<h3>About %L1 %L2</h3>"
    ""
    "<p>Copyright (C) %L3</p>"
    ""
    "<h4>Acknowledgments</h4>"
    ""
    "<p>GUI powered by "
    "<a href=\"http://www.qt-project.org/\">"
    "Qt</a>.</p>"
    ""
    "<p>Editor powered by "
    "<a href=\"http://www.qt-project.org/\">"
    "Qt Creator</a>.</p>"
    ""
    "<p>Qt plotting widget powered by "
    "<a href=\"http://www.qcustomplot.com/\">"
    "QCustomPlot</a>.</p>"
    ""
    "<p>FFT library powered by "
    "<a href=\"http://www.fftw.org/\">"
    "FFTW</a>.</p>"
    ""
    "<p>Build system powered by "
    "<a href=\"http://www.cmake.org/\">"
    "CMake</a>.</p>"
    ""
    "<p>Installer powered by "
    "<a href=\"http://installbuilder.bitrock.com/\">"
    "BitRock</a>.</p>"
    ""
    "<h4>Questions or Comments?</h4>"
    ""
    "<p>Contact us at "
    "<a href=\"mailto:%L4\">"
    "%L4</a>.</p>").
    arg(QApplication::applicationName()).
    arg(QApplication::applicationVersion()).
    arg(PROJECT_COPYRIGHT_STR).
    arg(PROJECT_EMAIL_STR));
}

void OmniaCreatorPlugin::processMessage(const int &code)
{
    if(!code)
    {
        QMessageBox::information(Core::ICore::mainWindow(),
        tr("New Version Available"), tr("A new version of %L1 is available").
        arg(QApplication::applicationName()));
    }
}

void OmniaCreatorPlugin::errorMessage(const QString &text)
{
    if(!text.isEmpty())
    {
        Core::MessageManager::write(text,
        Core::MessageManager::PrintToOutputPaneFlags(
        Core::MessageManager::ModeSwitch |
        Core::MessageManager::WithFocus |
        Core::MessageManager::EnsureSizeHint));
    }
}

void OmniaCreatorPlugin::deviceManagerSetup()
{
    // Init stuff...

    ProjectExplorer::DeviceManager *instance =
    ProjectExplorer::DeviceManager::instance();

    for(int i = 0, j = instance->deviceCount(); i < j; i++)
    {
        instance->removeDevice(instance->deviceAt(0)->id());
    }

    m_device = ProjectExplorer::IDevice::Ptr(new MicrocontrollerID);
    instance->addDevice(m_device);

    // Hide stuff...

    QObject *object = ExtensionSystem::PluginManager::getObjectByClassName
    ("ProjectExplorer::Internal::DeviceSettingsPage");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }
}

void OmniaCreatorPlugin::toolchainManagerSetup()
{
    // Init stuff...

    QList<ProjectExplorer::ToolChain *> list =
    ProjectExplorer::ToolChainManager::toolChains();

    foreach(ProjectExplorer::ToolChain *toolchain, list)
    {
        ProjectExplorer::ToolChainManager::deregisterToolChain(toolchain);
    }

    m_toolchain = new ProjectExplorer::GccToolChain(TOOLCHAIN_ID,
    ProjectExplorer::ToolChain::ManualDetection);

    if(!ProjectExplorer::ToolChainManager::registerToolChain(m_toolchain))
    {
        qFatal("Registering toolchain failed!");
    }

    // Hide stuff...

    QObject *object = ExtensionSystem::PluginManager::getObjectByClassName
    ("ProjectExplorer::Internal::ToolChainOptionsPage");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }
}

void OmniaCreatorPlugin::versionsManagerSetup()
{
    // Init stuff...

    QList<QtSupport::BaseQtVersion *> list =
    QtSupport::QtVersionManager::versions();

    foreach(QtSupport::BaseQtVersion *version, list)
    {
        QtSupport::QtVersionManager::removeVersion(version);
    }

    // Hide stuff...

    QObject *object = ExtensionSystem::PluginManager::getObjectByClassName
    ("QtSupport::Internal::QtOptionsPage");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }
}

void OmniaCreatorPlugin::kitManagerSetup()
{
    // Init stuff...

    QList<ProjectExplorer::Kit *> list =
    ProjectExplorer::KitManager::kits();

    foreach(ProjectExplorer::Kit *kit, list)
    {
        ProjectExplorer::KitManager::deregisterKit(kit);
    }

    m_kit = new ProjectExplorer::Kit(KIT_ID);
    m_kit->setDisplayName(m_toolchain->displayName());
    m_kit->setIconPath(Utils::FileName(QFileInfo(ICON_PATH)));

    ProjectExplorer::DeviceTypeKitInformation::setDeviceTypeId(m_kit,
    MICROCONTROLLER_DEVICE_TYPE);

    ProjectExplorer::DeviceKitInformation::setDevice(m_kit,
    m_device);

    ProjectExplorer::ToolChainKitInformation::setToolChain(m_kit,
    m_toolchain);

    if(!ProjectExplorer::KitManager::registerKit(m_kit))
    {
        qFatal("Registering kit failed!");
    }

    ProjectExplorer::KitManager::setDefaultKit(m_kit);

    // Hide stuff...

    QObject *object = ExtensionSystem::PluginManager::getObjectByClassName
    ("ProjectExplorer::KitOptionsPage");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }
}

void OmniaCreatorPlugin::cmakeManagerSetup()
{
    // Init stuff...

    Core::ICore::settings()->beginGroup("CMakeSettings");

    Core::ICore::settings()->setValue("cmakeExecutable",
    SerialMake::getCmakePath());

    Core::ICore::settings()->setValue("ninjaExecutable",
    SerialMake::getNinjaPath());

    Core::ICore::settings()->setValue("preferNinja", true);

    Core::ICore::settings()->endGroup();

    QObject *make = ExtensionSystem::PluginManager::getObjectByClassName
    ("CMakeProjectManager::Internal::CMakeManager");

    if(make)
    {
        static_cast<CMakeProjectManager::Internal::CMakeManager *>
        (make)->setCMakeExecutable(SerialMake::getCmakePath());

        static_cast<CMakeProjectManager::Internal::CMakeManager *>
        (make)->setNinjaExecutable(SerialMake::getNinjaPath());
    }

    // Hide stuff...

    QObject *object = ExtensionSystem::PluginManager::getObjectByClassName
    ("CMakeProjectManager::Internal::CMakeSettingsPage");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }
}

void OmniaCreatorPlugin::projectManagerSetup()
{
    // Init stuff...

    ProjectExplorer::Internal::ProjectExplorerSettings settings =
    ProjectExplorer::ProjectExplorerPlugin::
    instance()->projectExplorerSettings();

    settings.buildBeforeDeploy = false;
    settings.deployBeforeRun = false;
    settings.saveBeforeBuild = true;
    settings.showCompilerOutput = false;
    settings.showRunOutput = false;
    settings.showDebugOutput = false;
    settings.cleanOldAppOutput = false;
    settings.mergeStdErrAndStdOut = false;
    settings.wrapAppOutput = false;
    settings.useJom = false;
    settings.autorestoreLastSession = true;
    settings.prompToStopRunControl = false;
    settings.maxAppOutputLines = TERMINAL_MAX_LINE_SIZE;
    settings.environmentId = settings.environmentId;

    ProjectExplorer::ProjectExplorerPlugin::
    instance()->setProjectExplorerSettings(settings);

    Core::DocumentManager::setProjectsDirectory(m_make->getWorkspaceFolder());
    Core::DocumentManager::setUseProjectsDirectory(true);
    Core::DocumentManager::setBuildDirectory(m_make->getMakeBuildFolder());

    // Hide stuff...

    QObject *object = ExtensionSystem::PluginManager::getObjectByClassName
    ("ProjectExplorer::Internal::ProjectExplorerSettingsPage");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }

    object = ExtensionSystem::PluginManager::getObjectByClassName
    ("ProjectExplorer::Internal::AllProjectsFilter");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }

    object = ExtensionSystem::PluginManager::getObjectByClassName
    ("ProjectExplorer::Internal::CurrentProjectFilter");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }

    object = ExtensionSystem::PluginManager::getObjectByClassName
    ("ProjectExplorer::Internal::AllProjectsFind");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }

    object = ExtensionSystem::PluginManager::getObjectByClassName
    ("ProjectExplorer::Internal::CurrentProjectFind");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }

    object = ExtensionSystem::PluginManager::getObjectByClassName
    ("ProjectExplorer::Internal::ProjectTreeWidgetFactory");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }

    object = ExtensionSystem::PluginManager::getObjectByClassName
    ("ProjectExplorer::Internal::VcsAnnotateTaskHandler");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }

    object = ExtensionSystem::PluginManager::getObjectByClassName
    ("ProjectExplorer::Internal::AppOutputPane");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }

    foreach(Core::IMode *object,
    ExtensionSystem::PluginManager::getObjects<Core::IMode>())
    {
        if(QString(object->metaObject()->className())
        != QString("Core::Internal::EditMode"))
        {
            object->disconnect();
            ExtensionSystem::PluginManager::removeObject(object);
        }
    }

    connect(Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_RECENTPROJECTS)->menu(),
    SIGNAL(aboutToShow()), this, SLOT(updateRecentProjects()));
    updateRecentProjects();

    ///////////////////////////////////////////////////////////////////////////

    Core::ActionManager::actionContainer(
    Core::Constants::M_FILE)->menu()->removeAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::UNLOAD)->action());

    Core::ActionManager::actionContainer(
    Core::Constants::M_FILE)->menu()->removeAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::CLEARSESSION)->action());

    Core::ActionManager::actionContainer(
    Core::Constants::M_FILE)->menu()->removeAction(
    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_SESSION)->menu()->menuAction());

    Core::ActionManager::actionContainer(
    Core::Constants::M_FILE)->menu()->removeAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::NEWSESSION)->action());

    ///////////////////////////////////////////////////////////////////////////

    Core::ActionManager::command(
    ProjectExplorer::Constants::BUILDSESSION)->setDefaultKeySequence(
    Core::ActionManager::command(
    ProjectExplorer::Constants::BUILD)->defaultKeySequence());

    Core::ActionManager::command(
    ProjectExplorer::Constants::BUILD)->setDefaultKeySequence(
    QKeySequence());

    Core::ActionManager::command(
    ProjectExplorer::Constants::BUILDSESSION)->setKeySequence(
    Core::ActionManager::command(
    ProjectExplorer::Constants::BUILD)->keySequence());

    Core::ActionManager::command(
    ProjectExplorer::Constants::BUILD)->setKeySequence(
    QKeySequence());

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->removeAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::BUILD)->action());

    Core::ActionManager::command(
    ProjectExplorer::Constants::BUILDSESSION)->setAttribute(
    Core::Command::CA_UpdateText);

    static_cast<Utils::ProxyAction *>(Core::ActionManager::command(
    ProjectExplorer::Constants::BUILDSESSION)->action())->action()->setText(
    tr("Build Project"));

    static_cast<Utils::ProxyAction *>(Core::ActionManager::command(
    ProjectExplorer::Constants::BUILDSESSION)->action(
    ))->action()->setToolTip(
    static_cast<Utils::ProxyAction *>(Core::ActionManager::command(
    ProjectExplorer::Constants::BUILDSESSION)->action(
    ))->action()->toolTip());

    ///////////////////////////////////////////////////////////////////////////

    Core::ActionManager::command(
    CMakeProjectManager::Constants::RUNCMAKE)->action()->setIcon(
    QIcon(CMAKE_PATH));

    connect(ProjectExplorer::ProjectExplorerPlugin::instance(),
    SIGNAL(updateRunActions()), this, SLOT(updateRunCmake()));
    updateRunCmake();

    ///////////////////////////////////////////////////////////////////////////

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->removeAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::DEPLOYSESSION)->action());

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->removeAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::DEPLOY)->action());

    ///////////////////////////////////////////////////////////////////////////

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->removeAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::REBUILD)->action());

    Core::ActionManager::command(
    ProjectExplorer::Constants::REBUILDSESSION)->setAttribute(
    Core::Command::CA_UpdateText);

    static_cast<Utils::ProxyAction *>(Core::ActionManager::command(
    ProjectExplorer::Constants::REBUILDSESSION)->action())->action()->setText(
    tr("Rebuild Project"));

    static_cast<Utils::ProxyAction *>(Core::ActionManager::command(
    ProjectExplorer::Constants::REBUILDSESSION)->action(
    ))->action()->setToolTip(
    static_cast<Utils::ProxyAction *>(Core::ActionManager::command(
    ProjectExplorer::Constants::REBUILDSESSION)->action(
    ))->action()->toolTip());

    ///////////////////////////////////////////////////////////////////////////

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->removeAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::CLEAN)->action());

    Core::ActionManager::command(
    ProjectExplorer::Constants::CLEANSESSION)->setAttribute(
    Core::Command::CA_UpdateText);

    static_cast<Utils::ProxyAction *>(Core::ActionManager::command(
    ProjectExplorer::Constants::CLEANSESSION)->action())->action()->setText(
    tr("Clean Project"));

    static_cast<Utils::ProxyAction *>(Core::ActionManager::command(
    ProjectExplorer::Constants::CLEANSESSION)->action(
    ))->action()->setToolTip(
    static_cast<Utils::ProxyAction *>(Core::ActionManager::command(
    ProjectExplorer::Constants::CLEANSESSION)->action(
    ))->action()->toolTip());

    ///////////////////////////////////////////////////////////////////////////

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->removeAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::SELECTTARGET)->action());

    static_cast<Utils::ProxyAction *>(Core::ActionManager::command(
    ProjectExplorer::Constants::RUN)->action())->action()->setText(
    tr("Run Project"));

    static_cast<Utils::ProxyAction *>(Core::ActionManager::command(
    ProjectExplorer::Constants::RUN)->action(
    ))->action()->setToolTip(
    static_cast<Utils::ProxyAction *>(Core::ActionManager::command(
    ProjectExplorer::Constants::RUN)->action(
    ))->action()->toolTip());

    ///////////////////////////////////////////////////////////////////////////

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->setOnAllDisabledBehavior(
    Core::ActionContainer::Show);

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->removeAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::RUN)->action());

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->removeAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::BUILDSESSION)->action());

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->removeAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::REBUILDSESSION)->action());

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->removeAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::CLEANSESSION)->action());

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->removeAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::CANCELBUILD)->action());

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->removeAction(
    Core::ActionManager::command(
    CMakeProjectManager::Constants::RUNCMAKE)->action());

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->clear();

    ///////////////////////////////////////////////////////////////////////////

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->addAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::RUN)->action());

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->addAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::BUILDSESSION)->action());

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->addAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::REBUILDSESSION)->action());

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->addAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::CLEANSESSION)->action());

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->addSeparator();

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->addAction(
    Core::ActionManager::command(
    ProjectExplorer::Constants::CANCELBUILD)->action());

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->addSeparator();

    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_BUILDPROJECT)->menu()->addAction(
    Core::ActionManager::command(
    CMakeProjectManager::Constants::RUNCMAKE)->action());

    ///////////////////////////////////////////////////////////////////////////

    Core::ActionManager::actionContainer(
    Core::Constants::MENU_BAR)->menuBar()->removeAction(
    Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_DEBUG)->menu()->menuAction());

    ///////////////////////////////////////////////////////////////////////////

    QMainWindow *mainWindow =
    qobject_cast<QMainWindow *>
    (Core::ICore::mainWindow());

    if(mainWindow)
    {
        Core::Internal::FancyTabWidget *widget =
        qobject_cast<Core::Internal::FancyTabWidget *>
        (mainWindow->centralWidget());

        if(widget)
        {
            Core::Internal::FancyActionBar *actionBar0 =
            new Core::Internal::FancyActionBar(widget);

            widget->insertCornerWidget(0, actionBar0);

            actionBar0->insertAction(0,
            Core::ActionManager::command(Core::Constants::NEW)->action());

            actionBar0->insertAction(1,
            Core::ActionManager::command(Core::Constants::OPEN)->action());

            actionBar0->insertAction(2,
            Core::ActionManager::command(Core::Constants::SAVE)->action());

            ///////////////////////////////////////////////////////////////////

            Core::Internal::FancyActionBar *actionBar1 =
            new Core::Internal::FancyActionBar(widget);

            widget->insertCornerWidget(1, actionBar1);

            actionBar1->insertAction(0,
            Core::ActionManager::command(Core::Constants::UNDO)->action());

            actionBar1->insertAction(1,
            Core::ActionManager::command(Core::Constants::REDO)->action());

            actionBar1->insertAction(2,
            Core::ActionManager::command(Core::Constants::CUT)->action());

            actionBar1->insertAction(3,
            Core::ActionManager::command(Core::Constants::COPY)->action());

            actionBar1->insertAction(4,
            Core::ActionManager::command(Core::Constants::PASTE)->action());

            ///////////////////////////////////////////////////////////////////

            Core::Internal::FancyActionBar *actionBar2 =
            new Core::Internal::FancyActionBar(widget);

            widget->insertCornerWidget(2, actionBar2);

            actionBar2->insertAction(0,
            Core::ActionManager::command(
            ProjectExplorer::Constants::RUN)->action());

            actionBar2->insertAction(1,
            Core::ActionManager::command(
            ProjectExplorer::Constants::BUILDSESSION)->action());

            ///////////////////////////////////////////////////////////////////

            // Core::Internal::FancyActionBar *actionBar3 =
            // new Core::Internal::FancyActionBar(widget);

            // widget->insertCornerWidget(3, actionBar3);
        }
    }
}

void OmniaCreatorPlugin::environmentSetup()
{
    Core::ActionManager::actionContainer(
    Core::Constants::M_WINDOW)->menu()->removeAction(
    Core::ActionManager::command(
    Core::Constants::TOGGLE_MODE_SELECTOR)->action());

    QObject *object = ExtensionSystem::PluginManager::getObjectByClassName
    ("Core::Internal::MimeTypeSettings");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }

    object = ExtensionSystem::PluginManager::getObjectByClassName
    ("CMakeProjectManager::Internal::CMakeLocatorFilter");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }
}

void OmniaCreatorPlugin::textEditorCppSetup()
{
    Core::ActionManager::actionContainer(
    CppTools::Constants::M_TOOLS_CPP)->menu()->removeAction(
    Core::ActionManager::command(
    CppEditor::Constants::OPEN_PREPROCESSOR_DIALOG)->action());

    Core::ActionManager::actionContainer(
    CppTools::Constants::M_TOOLS_CPP)->menu()->removeAction(
    Core::ActionManager::command(
    CppEditor::Constants::INSPECT_CPP_CODEMODEL)->action());

    QObject *object = ExtensionSystem::PluginManager::getObjectByClassName
    ("TextEditor::HighlighterSettingsPage");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }

    // Doesn't actually work... had to comment out in code...
    object = ExtensionSystem::PluginManager::getObjectByClassName
    ("CppTools::Internal::Ui::CppFileSettingsPage");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }

    // Doesn't actually work... had to comment out in code...
    object = ExtensionSystem::PluginManager::getObjectByClassName
    ("CppTools::Internal::Ui::CppCodeModelSettingsPage");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }

    object = ExtensionSystem::PluginManager::getObjectByClassName
    ("CppEditor::Internal::ConvertCStringToNSString");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }

    object = ExtensionSystem::PluginManager::getObjectByClassName
    ("CppEditor::Internal::TranslateStringLiteral");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }

    object = ExtensionSystem::PluginManager::getObjectByClassName
    ("CppEditor::Internal::WrapStringLiteral");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }

    object = ExtensionSystem::PluginManager::getObjectByClassName
    ("CppEditor::Internal::InsertQtPropertyMembers");

    if(object)
    {
        ExtensionSystem::PluginManager::removeObject(object);
    }
}

void OmniaCreatorPlugin::updateRecentProjects()
{
    QRegularExpression re("set\\(PROJECT_.*?\"(?<path>.*?)\"\\)");

    foreach(QAction *action, Core::ActionManager::actionContainer(
    ProjectExplorer::Constants::M_RECENTPROJECTS)->menu()->actions())
    {
        if((!action->isSeparator())
        && (action->text() != Core::Constants::TR_CLEAR_MENU))
        {
            QFile file(action->text());

            if(file.open(QIODevice::ReadOnly))
            {
                action->setText(re.match(file.readAll()).captured("path"));
            }
        }
    }
}

void OmniaCreatorPlugin::updateRunCmake()
{
    Core::ActionManager::command(
    CMakeProjectManager::Constants::RUNCMAKE)->action()->setEnabled(
    Core::ActionManager::command(
    ProjectExplorer::Constants::RUN)->action()->isEnabled());
}

void OmniaCreatorPlugin::closeProject()
{
    ProjectExplorer::Project *project =
    ProjectExplorer::SessionManager::startupProject();

    if(!project)
    {
        ProjectExplorer::Project *project =
        ProjectExplorer::ProjectExplorerPlugin::currentProject();

        if(!project)
        {
            return;
        }
    }

    ProjectExplorer::ProjectExplorerPlugin::
    instance()->unloadProject(project);
}

void OmniaCreatorPlugin::openProject()
{
    qputenv("OC_WORKSPACE_FOLDER", m_make->getWorkspaceFolder().toUtf8());
    qputenv("OC_PROJECT_FOLDER", m_make->getProjectFolder().toUtf8());
    qputenv("OC_SERIAL_PORT", m_make->getProjectPortName().toUtf8());
    qputenv("OC_MAKE_FILE", m_make->getProjectMakeFile().toUtf8());

    Core::DocumentManager::setProjectsDirectory(
    m_make->getWorkspaceFolder());

    Core::DocumentManager::setBuildDirectory(
    m_make->getMakeBuildFolder());

    if(QFileInfo(m_make->getMakeFile()).exists())
    {
        ProjectExplorer::ProjectExplorerPlugin::
        instance()->openProject(m_make->getMakeFile(), NULL);
    }
}

void OmniaCreatorPlugin::cmakeChanged()
{
    closeProject();
    openProject();
}

void OmniaCreatorPlugin::buildClicked()
{
    CMakeProjectManager::Internal::CMakeProject *project =
    qobject_cast<CMakeProjectManager::Internal::CMakeProject *>
    (ProjectExplorer::SessionManager::startupProject());

    if(!project)
    {
        CMakeProjectManager::Internal::CMakeProject *project =
        qobject_cast<CMakeProjectManager::Internal::CMakeProject *>
        (ProjectExplorer::ProjectExplorerPlugin::currentProject());

        if(!project)
        {
            QMessageBox::critical(Core::ICore::mainWindow(),
            tr("Build Failed"), tr("Build project not set"));

            return;
        }
    }

    ProjectExplorer::Target *target =
    project->activeTarget();

    if(!target)
    {
        QMessageBox::critical(Core::ICore::mainWindow(),
        tr("Build Failed"), tr("Build target not set"));

        return;
    }

    ProjectExplorer::BuildConfiguration *configuration =
    target->activeBuildConfiguration();

    if(!configuration)
    {
        QMessageBox::critical(Core::ICore::mainWindow(),
        tr("Build Failed"), tr("Build configuration not set"));

        return;
    }

    ProjectExplorer::BuildStepList *list =
    configuration->stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD);

    if(!list)
    {
        QMessageBox::critical(Core::ICore::mainWindow(),
        tr("Build Failed"), tr("Build list not set"));

        return;
    }

    CMakeProjectManager::Internal::MakeStep *step =
    qobject_cast<CMakeProjectManager::Internal::MakeStep *>(list->at(0));

    if(!step)
    {
        QMessageBox::critical(Core::ICore::mainWindow(),
        tr("Build Failed"), tr("Build step not set"));

        return;
    }

    step->clearBuildTargets();

    if(project->hasBuildTarget("all"))
    {
        step->setBuildTarget("all", true);
    }
    else
    {
        QMessageBox::critical(Core::ICore::mainWindow(),
        tr("Build Failed"), tr("Build not set"));

        return;
    }

    ProjectExplorer::ProjectExplorerPlugin::instance()->buildProject(project);
}

void OmniaCreatorPlugin::runClicked()
{
    CMakeProjectManager::Internal::CMakeProject *project =
    qobject_cast<CMakeProjectManager::Internal::CMakeProject *>
    (ProjectExplorer::SessionManager::startupProject());

    if(!project)
    {
        CMakeProjectManager::Internal::CMakeProject *project =
        qobject_cast<CMakeProjectManager::Internal::CMakeProject *>
        (ProjectExplorer::ProjectExplorerPlugin::currentProject());

        if(!project)
        {
            QMessageBox::critical(Core::ICore::mainWindow(),
            tr("Run Failed"), tr("Run project not set"));

            return;
        }
    }

    ProjectExplorer::Target *target =
    project->activeTarget();

    if(!target)
    {
        QMessageBox::critical(Core::ICore::mainWindow(),
        tr("Run Failed"), tr("Run target not set"));

        return;
    }

    ProjectExplorer::BuildConfiguration *configuration =
    target->activeBuildConfiguration();

    if(!configuration)
    {
        QMessageBox::critical(Core::ICore::mainWindow(),
        tr("Run Failed"), tr("Run configuration not set"));

        return;
    }

    ProjectExplorer::BuildStepList *list =
    configuration->stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD);

    if(!list)
    {
        QMessageBox::critical(Core::ICore::mainWindow(),
        tr("Run Failed"), tr("Run list not set"));

        return;
    }

    CMakeProjectManager::Internal::MakeStep *step =
    qobject_cast<CMakeProjectManager::Internal::MakeStep *>(list->at(0));

    if(!step)
    {
        QMessageBox::critical(Core::ICore::mainWindow(),
        tr("Run Failed"), tr("Run step not set"));

        return;
    }

    step->clearBuildTargets();

    if(project->hasBuildTarget("upload"))
    {
        step->setBuildTarget("upload", true);
    }
    else
    {
        QMessageBox::critical(Core::ICore::mainWindow(),
        tr("Run Failed"), tr("Run not set"));

        return;
    }

    m_port->getPort()->close();

    ProjectExplorer::BuildManager *manager =
    qobject_cast<ProjectExplorer::BuildManager *>
    (ProjectExplorer::BuildManager::instance());

    connect(manager, SIGNAL(buildQueueFinished(bool)),
            this, SLOT(reopenPort()));

    ProjectExplorer::ProjectExplorerPlugin::instance()->buildProject(project);
}

void OmniaCreatorPlugin::reopenPort()
{
    QString port = m_port->getLastPort();

    if(m_port->portInDemoPortList(port)
    || m_port->portInSerialPortList(port))
    {
        m_escape->setPort(m_port->openPort(port, true));
    }

    ProjectExplorer::BuildManager *manager =
    qobject_cast<ProjectExplorer::BuildManager *>
    (ProjectExplorer::BuildManager::instance());

    disconnect(manager, SIGNAL(buildQueueFinished(bool)),
               this, SLOT(reopenPort()));
}

void OmniaCreatorPlugin::messageHandler(QtMsgType type, const char *text)
{
    QStringList ignored;

    ignored << "No tool chain set from kit \"GCC\".";
    ignored << "PluginManagerPrivate::removeObject(): object not in list:";
    ignored << "static Core::IEditor* Core::EditorManager::createEditor(const"
               " Core::Id&, const QString&) unable to determine mime type of";
    ignored << "QTextStream: No device"; // This might not be harmless...
    ignored << "SOFT ASSERT: \"existingParamCount == "
               "declaredParameterCount(targetFunction)\" in file";
               // This might not be harmless...

    foreach(const QString &string, ignored)
    {
        if(QString(text).startsWith(string))
        {
            return;
        }
    }

    switch(type)
    {
        case QtDebugMsg:
        {
            fprintf(stderr, "%s\n", text); fflush(stderr); break;
        }

        case QtWarningMsg:
        {
            fprintf(stderr, "%s\n", text); fflush(stderr); break;
        }

        case QtCriticalMsg:
        {
            fprintf(stderr, "%s\n", text); fflush(stderr); break;
        }

        case QtFatalMsg:

        default:
        {
            fprintf(stderr, "%s\n", text); fflush(stderr); abort();
        }
    }
}

Q_EXPORT_PLUGIN2(OmniaCreator, OmniaCreatorPlugin)

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
