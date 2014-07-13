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
    // UNUSED //
}

OmniaCreatorPlugin::~OmniaCreatorPlugin()
{
    // UNUSED //
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

    // Serial Port Init ///////////////////////////////////////////////////////

    m_port = new SerialPort(Core::ICore::mainWindow(),
                            Core::ICore::settings(), this);

    m_make->setSerialPort(m_port);
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
    if(!m_make->getWorkspaceFolderWasSet())
    {
        UtilPathPicker picker(tr("Select Workspace Location"),
        tr("Please select a workspace location"),
        m_make->getWorkspaceFolder(), Core::ICore::mainWindow());

        if(picker.exec() == QDialog::Accepted)
        {
            m_make->setWorkspaceFolder(picker.getDefaultDir());
        }
    }

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

    return true;
}

ExtensionSystem::IPlugin::ShutdownFlag OmniaCreatorPlugin::aboutToShutdown()
{
    delete m_escape;
    delete m_port;
    delete m_make;

    SerialOscilloscope::finiFftw();

    return ExtensionSystem::IPlugin::SynchronousShutdown;
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
    if(!QDesktopServices::openUrl(QUrl(PROJECT_URL_STR
    "help/general/")))
    {
        QMessageBox::critical(Core::ICore::mainWindow(),
        tr("Open General Help Error"),
        tr("Unable to open the URL to the General Help page"));
    }
}

void OmniaCreatorPlugin::editorHelp()
{
    if(!QDesktopServices::openUrl(QUrl(PROJECT_URL_STR
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
    "<p>Copyright (c) %L3</p>"
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
