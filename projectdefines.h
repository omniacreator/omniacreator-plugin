/***************************************************************************//**
* @file
* Project Defines
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

#ifndef PROJECTDEFINES_H
#define PROJECTDEFINES_H

#include "projectconstants.h"

// Plugin Dialog //////////////////////////////////////////////////////////////

#define PLUGIN_DIALOG "PluginDialog"

#define PLUGIN_DIALOG_KEY_GROUP \
(PROJECT_FULL_NAME_WO_SPACES_STR "/" PLUGIN_DIALOG)

#define PLUGIN_DIALOG_KEY_INITIAL_SETTINGS "initialSettings"

#define PLUGIN_DIALOG_KEY_NEW_SOFTWARE "newSoftware"

#define PLUGIN_DIALOG_KEY_NEW_FILE "newFile"
#define PLUGIN_DIALOG_KEY_NEW_PROJECT "newProject"
#define PLUGIN_DIALOG_KEY_OPEN_FILE "openFile"
#define PLUGIN_DIALOG_KEY_OPEN_PROJECT "openProject"

// Serial Make ////////////////////////////////////////////////////////////////

#define SERIAL_MAKE "SerialMake"

#define SERIAL_MAKE_KEY_GROUP \
(PROJECT_FULL_NAME_WO_SPACES_STR "/" SERIAL_MAKE)

#define SERIAL_MAKE_KEY_WORKSPACE_FOLDER "workspaceFolder"
#define SERIAL_MAKE_KEY_PROJECT_FOLDER "projectFolder"
#define SERIAL_MAKE_KEY_PROJECT_PORT_NAME "projectPortName"
#define SERIAL_MAKE_KEY_PROJECT_MAKE_FILE "projectMakeFile"

// Serial Port ////////////////////////////////////////////////////////////////

#define SERIAL_PORT "SerialPort"

#define SERIAL_PORT_KEY_GROUP \
(PROJECT_FULL_NAME_WO_SPACES_STR "/" SERIAL_PORT)

#define SERIAL_PORT_KEY_PORT_OPEN "portOpen"
#define SERIAL_PORT_KEY_PORT_LIST "portList"

#define SERIAL_PORT_KEY_PRETTY_NAME "prettyName"
#define SERIAL_PORT_KEY_MAKE_FILE "makeFile"
#define SERIAL_PORT_KEY_BAUD_RATE "baudRate"
#define SERIAL_PORT_KEY_LATENCY_TIMER "latencyTimer"
#define SERIAL_PORT_KEY_READ_TRANSFER_TIME "readTransferTime"
#define SERIAL_PORT_KEY_WRITE_TRANSFER_TIME "writeTransferTime"
#define SERIAL_PORT_KEY_READ_TRANSFER_SIZE "readTransferSize"
#define SERIAL_PORT_KEY_WRITE_TRANSFER_SIZE "writeTransferSize"

// Serial Escape //////////////////////////////////////////////////////////////

#define SERIAL_ESCAPE "SerialEscape"

#define SERIAL_ESCAPE_KEY_GROUP \
(PROJECT_FULL_NAME_WO_SPACES_STR "/" SERIAL_ESCAPE)

#define SERIAL_ESCAPE_KEY_OPEN_PROCESS_EN "openProcessEn"
#define SERIAL_ESCAPE_KEY_OPEN_URL_EN "openUrlEn"

#define SERIAL_ESCAPE_JSON "SerialEscapeJSON"

#define SERIAL_ESCAPE_JSON_KEY_GROUP \
(PROJECT_FULL_NAME_WO_SPACES_STR "/" SERIAL_ESCAPE_JSON)

#define SERIAL_ESCAPE_JSON_KEY_FILE "file"

// Serial Terminal ////////////////////////////////////////////////////////////

#define SERIAL_TERMINAL "SerialTerminal"

#define SERIAL_TERMINAL_KEY_GROUP \
(PROJECT_FULL_NAME_WO_SPACES_STR "/" SERIAL_TERMINAL)

#define SERIAL_TERMINAL_KEY_STATE "state"
#define SERIAL_TERMINAL_KEY_GEOMETRY "geometry"

#define SERIAL_TERMINAL_KEY_OPEN_FILE "openFile"
#define SERIAL_TERMINAL_KEY_SAVE_FILE "saveFile"

// Serial Oscilloscope ////////////////////////////////////////////////////////

#define SERIAL_OSCILLOSCOPE "SerialOscilloscope"

#define SERIAL_OSCILLOSCOPE_KEY_GROUP \
(PROJECT_FULL_NAME_WO_SPACES_STR "/" SERIAL_OSCILLOSCOPE)

#define SERIAL_OSCILLOSCOPE_KEY_STATE "state"
#define SERIAL_OSCILLOSCOPE_KEY_GEOMETRY "geometry"

#define SERIAL_OSCILLOSCOPE_KEY_SAVE_R_FILE "saveRasterFile"
#define SERIAL_OSCILLOSCOPE_KEY_SAVE_V_FILE "saveVectorFile"

#define SERIAL_OSCILLOSCOPE_KEY_IMPORT_FILE "importFile"
#define SERIAL_OSCILLOSCOPE_KEY_EXPORT_FILE "exportFile"

#define SERIAL_OSCILLOSCOPE_FFTW "SerialOscilloscopeFFTW"

#define SERIAL_OSCILLOSCOPE_FFTW_KEY_GROUP \
(PROJECT_FULL_NAME_WO_SPACES_STR "/" SERIAL_OSCILLOSCOPE_FFTW)

#define SERIAL_OSCILLOSCOPE_FFTW_KEY_WISDOM "wisdom"

// Serial Tree ////////////////////////////////////////////////////////////////

#define SERIAL_TREE "SerialTree"

#define SERIAL_TREE_KEY_GROUP \
(PROJECT_FULL_NAME_WO_SPACES_STR "/" SERIAL_TREE)

#define SERIAL_TREE_KEY_STATE "state"
#define SERIAL_TREE_KEY_GEOMETRY "geometry"

#define SERIAL_TREE_KEY_OPEN_FILE "openFile"
#define SERIAL_TREE_KEY_SAVE_FILE "saveFile"

// Serial Table ///////////////////////////////////////////////////////////////

#define SERIAL_TABLE "SerialTable"

#define SERIAL_TABLE_KEY_GROUP \
(PROJECT_FULL_NAME_WO_SPACES_STR "/" SERIAL_TABLE)

#define SERIAL_TABLE_KEY_STATE "state"
#define SERIAL_TABLE_KEY_GEOMETRY "geometry"

#define SERIAL_TABLE_KEY_OPEN_FILE "openFile"
#define SERIAL_TABLE_KEY_SAVE_FILE "saveFile"

// Serial Interface ///////////////////////////////////////////////////////////

#define SERIAL_INTERFACE "SerialInterface"

#define SERIAL_INTERFACE_KEY_GROUP \
(PROJECT_FULL_NAME_WO_SPACES_STR "/" SERIAL_INTERFACE)

#define SERIAL_INTERFACE_KEY_STATE "state"
#define SERIAL_INTERFACE_KEY_GEOMETRY "geometry"

// Serial Graphics ////////////////////////////////////////////////////////////

#define SERIAL_GRAPHICS "SerialGraphics"

#define SERIAL_GRAPHICS_KEY_GROUP \
(PROJECT_FULL_NAME_WO_SPACES_STR "/" SERIAL_GRAPHICS)

#define SERIAL_GRAPHICS_KEY_STATE "state"
#define SERIAL_GRAPHICS_KEY_GEOMETRY "geometry"

#define SERIAL_GRAPHICS_KEY_SAVE_R_FILE "saveRasterFile"
#define SERIAL_GRAPHICS_KEY_SAVE_V_FILE "saveVectorFile"

#define SERIAL_GRAPHICS_KEY_IMPORT_FILE "importFile"
#define SERIAL_GRAPHICS_KEY_EXPORT_FILE "exportFile"

#endif // PROJECTDEFINES_H

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
