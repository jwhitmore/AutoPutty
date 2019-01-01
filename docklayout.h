/*
 * docklayout.h:
 * AutoPutty: Auto-logon/Tabbed PuTTy
 * Copyright (c) 2017-2019 Justin Whitmore/Travis Kramper
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

#ifndef DOCKLAYOUT_H
#define DOCKLAYOUT_H

#include <QtWidgets/QMainWindow>
#include <QString>
#include <QStringList>
#include <QSettings>
#include <QVector>

struct SessionCfg {
  SessionCfg() {sessionConfigId = -1;}
  SessionCfg(QString LayoutObjectName, QString PuttySessionName, QString PuttyObjectName, int configID = -1)
    {
      layoutObjectName  = LayoutObjectName;
      sessionName       = PuttySessionName;
      sessionObjectName = PuttyObjectName;
      sessionConfigId   = configID;
    }
  QString layoutObjectName;
  QString sessionName;
  QString sessionObjectName;
  int sessionConfigId;
};
typedef SessionCfg SessionCfg;

class dockLayout
{
public:

  dockLayout(QString orgName, QString appName);

  QStringList getLayoutNames() const;
  QStringList getLayoutSessionNames(QString layout) const;
  QVector<SessionCfg> getLayoutSessions(QString layout);
  QByteArray getLayoutGeometry(QString layout) const;
  QByteArray getLayoutState(QString layout) const;
  QString getLayoutObjectName(QString layout);

  bool exists(QString layout);

  void deleteLayout(QString layout);
  void renameLayout(QString layout, QString newName);

private:
  QSettings *settings;

};

#endif // DOCKLAYOUT_H
