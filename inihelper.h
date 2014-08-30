/*
 * inihelper.h:
 * AutoPutty: Auto-logon/Tabbed PuTTy
 * Copyright (c) 2012-2014 Justin Whitmore
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

#ifndef INIHELPER_H
#define INIHELPER_H
#include <QSettings>
#include <QVariant>
#include <QFile>

class INIHelper
{
public:
  INIHelper();
  ~INIHelper();
  INIHelper(QString iniFile);
  void initialize(QString iniFile);
  QString readValue(QString section, QString key);
  QByteArray readByteArray(QString section, QString key);
  bool writeValue(QString section, QString key, QVariant value);

private:
  QSettings* iniSettings;
  QFile iniFile;
};

#endif // INIHELPER_H
