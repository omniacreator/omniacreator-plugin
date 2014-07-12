/***************************************************************************//**
* @file
* Project Constants
*
* @version @n 1.0
* @date @n 6/28/2014
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2014 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 6/28/2014
*******************************************************************************/

#ifndef PROJECTCONSTANTS_H
#define PROJECTCONSTANTS_H

#define __STRINGIFY__(x) #x
#define STRINGIFY(x) __STRINGIFY__(x)

#define PROJECT_FULL_NAME_STR STRINGIFY(PROJECT_FULL_NAME)
#define PROJECT_FULL_NAME_WO_SPACES_STR STRINGIFY(PROJECT_FULL_NAME_WO_SPACES)
#define PROJECT_SHORT_NAME_STR STRINGIFY(PROJECT_SHORT_NAME)
#define PROJECT_VERSION_STR STRINGIFY(PROJECT_VERSION)
#define PROJECT_VENDOR_STR STRINGIFY(PROJECT_VENDOR)
#define PROJECT_COPYRIGHT_STR STRINGIFY(PROJECT_COPYRIGHT)
#define PROJECT_CATEGORY_STR STRINGIFY(PROJECT_CATEGORY)
#define PROJECT_DESCRIPTION_STR STRINGIFY(PROJECT_DESCRIPTION)
#define PROJECT_DOMAIN_NAME_STR STRINGIFY(PROJECT_DOMAIN_NAME)
#define PROJECT_URL_STR STRINGIFY(PROJECT_URL)
#define PROJECT_URL_WO_SLASH_STR STRINGIFY(PROJECT_URL_WO_SLASH)
#define PROJECT_EMAIL_STR STRINGIFY(PROJECT_EMAIL)

// Board Menu /////////////////////////////////////////////////////////////////

const char BOARD_MENU_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".BoardMenu";

const char CHANGE_SERIAL_PORT_BOARD_NAME_ACTION_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".ChangeSerialPortBoardNameAction";

const char CHANGE_SERIAL_PORT_BOARD_TYPE_ACTION_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".ChangeSerialPortBoardTypeAction";

const char SERIAL_PORT_MENU_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".SerialPortMenu";

const char BAUD_RATE_MENU_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".BaudRateMenu";

const char RESET_SERIAL_PORT_ACTION_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".ResetSerialPort";

// Widgets Menu ///////////////////////////////////////////////////////////////

const char WIDGETS_MENU_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".WidgetsMenu";

const char EXPORT_WIDGET_STATE_MENU_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".ExportWidgetStateMenu";

const char IMPORT_WIDGET_STATE_ACTION_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".ImportWidgetStateAction";

const char REMOVE_WIDGET_MENU_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".RemoveWidgetMenu";

const char REMOVE_ALL_WIDGETS_ACTION_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".RemoveAllWidgetsAction";

const char RESET_SERIAL_TERMINAL_ACTION_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".ResetSerialTerminal";

// Help Menu //////////////////////////////////////////////////////////////////

const char GENERAL_HELP_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".GeneralHelpAction";

const char EDITOR_HELP_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".EditorHelpAction";

const char ABOUT_ACTION_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".AboutAction";

const char ABOUT_QT_ACTION_ID[] =
PROJECT_FULL_NAME_WO_SPACES_STR ".AboutQtAction";

#endif // PROJECTCONSTANTS_H

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
