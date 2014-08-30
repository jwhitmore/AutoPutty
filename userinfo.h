/*
 * userinfo.h:
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

#ifndef USERINFO_H
#define USERINFO_H

#include <QStringList>
#include <QMap>

struct user_struct {
  QString username;
  QString password;
};

typedef QMap<QString,user_struct> USER_MAP;

class UserInfo
{
public:
  UserInfo();
  ~UserInfo();
  typedef USER_MAP::iterator iterator;
  typedef const USER_MAP::const_iterator const_iterator;
  iterator begin() { return map.begin(); }
  iterator end() { return map.end(); }
  bool isEmpty();
  void clear();
  void addUser(QString key, user_struct user);
  void addUser(QString key, QString userName, QString password);
  UserInfo::iterator find(QString key);
  void removeUser(QString key);
  USER_MAP getUserMap();
  static UserInfo* getUserMap(QString iniFile, QStringList keys, QString passphrase);
private:
  USER_MAP map;

};

#endif // USERINFO_H
