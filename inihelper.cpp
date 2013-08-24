/*
 * inihelper.cpp:
 * AutoPutty: Auto-logon/Tabbed PuTTy
 * Copyright (c) 2012-2013 Justin Whitmore
 ***********************************************************************
 * This file is part of AutoPutty:
 *
 * AutoPutty is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AutoPutty is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
*/

#include <stdexcept>
#include "debug.h"
#include "inihelper.h"

//=============================================================================
//=============================================================================
INIHelper::INIHelper()
{
  FUNC_DEBUG;
  iniSettings = NULL;
}

//=============================================================================
//=============================================================================
INIHelper::INIHelper(QString iniFile)
{
  FUNC_DEBUG;
  iniSettings = NULL;
  initialize(iniFile);
}

//=============================================================================
//=============================================================================
INIHelper::~INIHelper()
{
  FUNC_DEBUG;
  if (iniSettings == NULL) {
    delete iniSettings;
  }
}

//=============================================================================
// Checks if INI File exists and creates if does not exist and
// sets up object to point to INI File
//=============================================================================
void INIHelper::initialize(QString iniFile)
{
  FUNC_DEBUG;
  try
  {
    if (!QFile::exists(iniFile)) {
      QFile file(iniFile);
      file.open(QIODevice::WriteOnly);
      file.close();
    }
    if (iniSettings == NULL) {
      iniSettings = new QSettings(iniFile,QSettings::IniFormat);
    }
  } catch (std::exception &e)
  {
    DEBUG << "exception " << e.what();
  }
}

//=============================================================================
// Returns value of INI field
//=============================================================================
QString INIHelper::readValue(QString section, QString key)
{
  FUNC_DEBUG;
  try
  {
    if (iniSettings != NULL) {
      iniSettings->sync();
      iniSettings->beginGroup(section);
      QVariant value = QVariant(iniSettings->value(key));
      iniSettings->endGroup();
      return value.toString();
    }
  } catch (std::exception &e) {
    DEBUG << "exception " << e.what();
  }
  return QString();
}

//=============================================================================
// Returns value of INI field that is a QByteArray
//=============================================================================
QByteArray INIHelper::readByteArray(QString section, QString key)
{
  FUNC_DEBUG;
  try
  {
    if (iniSettings != NULL) {
      iniSettings->sync();
      iniSettings->beginGroup(section);
      QVariant value = QVariant(iniSettings->value(key));
      iniSettings->endGroup();
      return value.toByteArray();
    }
  } catch (std::exception &e) {
    DEBUG << "exception " << e.what();
  }
  return QByteArray();
}

//=============================================================================
// Writes Section:Name:Value to INI File
//=============================================================================
bool INIHelper::writeValue(QString section, QString key, QVariant value)
{
  FUNC_DEBUG;
  try {
    if (iniSettings != NULL) {
      iniSettings->beginGroup(section);
      iniSettings->setValue(key,value);
      iniSettings->endGroup();
      iniSettings->sync();
      return true;
    }
  } catch (std::exception &e) {
    DEBUG << "exception " << e.what();
  }
  return false;
}
