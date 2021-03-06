################################################################################
# @file
# Omnia Creator
#
# @version @n 1.0
# @date @n 6/28/2014
#
# @author @n Kwabena W. Agyeman
# @copyright @n (c) 2014 Kwabena W. Agyeman
# @n All rights reserved - Please see the end of the file for the terms of use
#
# @par Update History:
# @n v1.0 - Original release - 6/28/2014
################################################################################

PROJECT_FULL_NAME = "Omnia Creator"
PROJECT_FULL_NAME_WO_SPACES = "$$replace(PROJECT_FULL_NAME, "[ _]", "")"
PROJECT_SHORT_NAME = "$$lower($$PROJECT_FULL_NAME_WO_SPACES)"
PROJECT_VERSION = "0.3"
PROJECT_VENDOR = "$$PROJECT_FULL_NAME"
PROJECT_COPYRIGHT = "2014 Kwabena W. Agyeman"
PROJECT_CATEGORY = "Microcontrollers"
PROJECT_DESCRIPTION = "Omnia Creator is a cross platform C/C++ IDE for Makers"
PROJECT_DOMAIN_NAME = "$${PROJECT_SHORT_NAME}.com"
PROJECT_URL = "http://$${PROJECT_DOMAIN_NAME}/"
PROJECT_URL_WO_SLASH = "http://$${PROJECT_DOMAIN_NAME}"
PROJECT_EMAIL = "$${PROJECT_SHORT_NAME}@$${PROJECT_DOMAIN_NAME}"

include(../../qtcreatorplugin.pri)

QT += concurrent opengl printsupport serialport svg widgets

DEFINES += PROJECT_FULL_NAME=\"$$PROJECT_FULL_NAME\"
DEFINES += PROJECT_FULL_NAME_WO_SPACES=\"$$PROJECT_FULL_NAME_WO_SPACES\"
DEFINES += PROJECT_SHORT_NAME=\"$$PROJECT_SHORT_NAME\"
DEFINES += PROJECT_VERSION=\"$$PROJECT_VERSION\"
DEFINES += PROJECT_VENDOR=\"$$PROJECT_VENDOR\"
DEFINES += PROJECT_COPYRIGHT=\"$$PROJECT_COPYRIGHT\"
DEFINES += PROJECT_CATEGORY=\"$$PROJECT_CATEGORY\"
DEFINES += PROJECT_DESCRIPTION=\"$$PROJECT_DESCRIPTION\"
DEFINES += PROJECT_DOMAIN_NAME=\"$$PROJECT_DOMAIN_NAME\"
DEFINES += PROJECT_URL=\"$$PROJECT_URL\"
DEFINES += PROJECT_URL_WO_SLASH=\"$$PROJECT_URL_WO_SLASH\"
DEFINES += PROJECT_EMAIL=\"$$PROJECT_EMAIL\"

DEFINES -= QT_NO_CAST_FROM_ASCII
DEFINES -= QT_NO_CAST_TO_ASCII
DEFINES -= QT_NO_CAST_FROM_BYTEARRAY
DEFINES += QT_USE_FAST_CONCATENATION
DEFINES += QT_USE_FAST_OPERATOR_PLUS
DEFINES += QT_USE_QSTRINGBUILDER

HEADERS += omniacreatorplugin.h \
           omniacreator_global.h \
           projectconstants.h \
           projectdefines.h \
           serialmake.h \
           serialport.h \
           serialdevice.h \
           serialescape.h \
           serialwindow.h \
           serialgraphics.h \
           serialinterface.h \
           serialoscilloscope.h \
           serialtable.h \
           serialterminal.h \
           serialtree.h \
           newfiledialog.h \
           newprojectdialog.h \
           newfileorprojectdialog.h \
           openfileorprojectdialog.h \
           utilitempicker.h \
           utilpathpicker.h \
           utilcolumndialog.h \
           utilrasterdialog.h \
           utilvectordialog.h \
           utilvectordialogwowh.h \
           demoport.h \
           parallax/openspinparser.h \
           parallax/proploadparser.h

SOURCES += omniacreatorplugin.cpp \
           serialmake.cpp \
           serialport.cpp \
           serialdevice.cpp \
           serialescape.cpp \
           serialwindow.cpp \
           serialgraphics.cpp \
           serialinterface.cpp \
           serialoscilloscope.cpp \
           serialtable.cpp \
           serialterminal.cpp \
           serialtree.cpp \
           newfiledialog.cpp \
           newprojectdialog.cpp \
           newfileorprojectdialog.cpp \
           openfileorprojectdialog.cpp \
           utilitempicker.cpp \
           utilpathpicker.cpp \
           utilcolumndialog.cpp \
           utilrasterdialog.cpp \
           utilvectordialog.cpp \
           utilvectordialogwowh.cpp \
           demoport.cpp \
           parallax/openspinparser.cpp \
           parallax/proploadparser.cpp

FORMS += serialgraphics.ui \
         serialinterface.ui \
         serialoscilloscope.ui \
         serialtable.ui \
         serialterminal.ui \
         serialtree.ui \
         newfiledialog.ui \
         newprojectdialog.ui \
         newfileorprojectdialog.ui \
         openfileorprojectdialog.ui \
         utilitempicker.ui \
         utilpathpicker.ui \
         utilcolumndialog.ui \
         utilrasterdialog.ui \
         utilvectordialog.ui \
         utilvectordialogwowh.ui

RESOURCES += omniacreator.qrc

# Include InterfaceLibrary

INCLUDEPATH += $$PWD/interfacelibrary
DEPENDPATH += $$PWD/interfacelibrary

HEADERS += $$PWD/interfacelibrary/il.h \
           $$PWD/interfacelibrary/ilshared.h \
           $$PWD/interfacelibrary/ildependencies.h \
           $$PWD/interfacelibrary/ilcore.h \
           $$PWD/interfacelibrary/ilterminal.h \
           $$PWD/interfacelibrary/iloscilloscope.h \
           $$PWD/interfacelibrary/iltable.h \
           $$PWD/interfacelibrary/iltree.h \
           $$PWD/interfacelibrary/ilgraphics.h \
           $$PWD/interfacelibrary/ilinterface.h \
           $$PWD/interfacelibrary/iloscilloscopebar.h \
           $$PWD/interfacelibrary/iloscilloscopecurve.h \
           $$PWD/interfacelibrary/iloscilloscopegraph.h \
           $$PWD/interfacelibrary/iloscilloscopebox.h \
           $$PWD/interfacelibrary/ilinterfacegroupbox.h \
           $$PWD/interfacelibrary/ilinterfacepushbutton.h \
           $$PWD/interfacelibrary/ilinterfaceradiobutton.h \
           $$PWD/interfacelibrary/ilinterfacecheckbox.h \
           $$PWD/interfacelibrary/ilinterfacelineedit.h \
           $$PWD/interfacelibrary/ilinterfaceintspinbox.h \
           $$PWD/interfacelibrary/ilinterfacebinspinbox.h \
           $$PWD/interfacelibrary/ilinterfacehexspinbox.h \
           $$PWD/interfacelibrary/ilinterfacedoublespinbox.h \
           $$PWD/interfacelibrary/ilinterfacetimeedit.h \
           $$PWD/interfacelibrary/ilinterfacedateedit.h \
           $$PWD/interfacelibrary/ilinterfacedatetimeedit.h \
           $$PWD/interfacelibrary/ilinterfaceslider.h \
           $$PWD/interfacelibrary/ilinterfacelabel.h \
           $$PWD/interfacelibrary/ilinterfaceprogressbar.h \
           $$PWD/interfacelibrary/ilinterfaceseperator.h

# Include BinaryEditor

INCLUDEPATH += $$PWD/../bineditor
DEPENDPATH += $$PWD/../bineditor

HEADERS += $$PWD/../bineditor/bineditor.h
SOURCES += $$PWD/../bineditor/bineditor.cpp

# Include QCustomPlot

INCLUDEPATH += $$PWD/qcustomplot
DEPENDPATH += $$PWD/qcustomplot

HEADERS += $$PWD/qcustomplot/qcustomplot.h
SOURCES += $$PWD/qcustomplot/qcustomplot.cpp

# Include FFTW3

win32: INCLUDEPATH += $$PWD/fftw3/windows/32
win64: INCLUDEPATH += $$PWD/fftw3/windows/64
macx: INCLUDEPATH += $$PWD/fftw3/mac
win32: DEPENDPATH += $$PWD/fftw3/windows/32
win64: DEPENDPATH += $$PWD/fftw3/windows/64
macx: DEPENDPATH += $$PWD/fftw3/mac

win32: HEADERS += $$PWD/fftw3/windows/32/fftw3.h
win64: HEADERS += $$PWD/fftw3/windows/64/fftw3.h
macx: HEADERS += $$PWD/fftw3/mac/fftw3.h

win32: LIBS += -L$$PWD/fftw3/windows/32 -llibfftw3-3
win64: LIBS += -L$$PWD/fftw3/windows/64 -llibfftw3-3
macx: LIBS += -L$$PWD/fftw3/mac -lfftw3

win32: OTHER_FILES += $$PWD/fftw3/windows/32/libfftw3-3.def
win64: OTHER_FILES += $$PWD/fftw3/windows/64/libfftw3-3.def
mac: OTHER_FILES += $$PWD/fftw3/mac/libfftw3.dylib
win32: OTHER_FILES += $$PWD/fftw3/windows/32/libfftw3-3.dll
win64: OTHER_FILES += $$PWD/fftw3/windows/64/libfftw3-3.dll
mac: OTHER_FILES += $$PWD/fftw3/mac/libfftw3.3.dylib

################################################################################
# @file
# @par MIT License - TERMS OF USE:
# @n Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# @n
# @n The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# @n
# @n THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
################################################################################
