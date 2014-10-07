#/*
# * AutoPutty.pro:
# * AutoPutty: Auto-logon/Tabbed PuTTy
# * Copyright (c) 2012-2014 Justin Whitmore
# ***********************************************************************
# * This file is part of AutoPutty:
# *
# * AutoPutty is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * AutoPutty is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ***********************************************************************
#*/
MAJOR = 5
MINOR = 0
PATCH = 6
VERSION_HEADER = version.h

exists(.git) {
  versiontarget.target = $$VERSION_HEADER
  versiontarget.commands = $$PWD\\version.exe $$MAJOR $$MINOR $$PATCH $$VERSION_HEADER GIT
  versiontarget.depends = FORCE

  PRE_TARGETDEPS += $$VERSION_HEADER
  QMAKE_EXTRA_TARGETS += versiontarget
} else {
  versiontarget.target = $$VERSION_HEADER
  versiontarget.commands = $$PWD\\version.exe $$MAJOR $$MINOR $$PATCH $$VERSION_HEADER
  versiontarget.depends = FORCE

  PRE_TARGETDEPS += $$VERSION_HEADER
  QMAKE_EXTRA_TARGETS += versiontarget
}
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AutoPutty

TEMPLATE = app

LIBS += -luser32

SOURCES +=  main.cpp\
            mainwindow.cpp \
            puttywidget.cpp \
            inihelper.cpp \
            actionwrapper.cpp \
            usersetup.cpp \
            userinfo.cpp \
            about.cpp \
            crypt.cpp

HEADERS  += mainwindow.h \
            puttywidget.h \
            inihelper.h \
            actionwrapper.h \
            usersetup.h \
            userinfo.h \
            debug.h \
            about.h \
            crypt.h

FORMS    += mainwindow.ui \
            usersetup.ui \
            about.ui

RESOURCES += \
            resources.qrc

OTHER_FILES += \
            resource.rc

RC_FILE = resource.rc

CONFIG += static release


