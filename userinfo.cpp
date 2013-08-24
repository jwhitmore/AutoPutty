/*
 * userinfo.cpp:
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

#include "crypt.h"
#include "debug.h"
#include "userinfo.h"
#include "inihelper.h"

//=============================================================================
//=============================================================================
UserInfo::UserInfo()
{
  FUNC_DEBUG;
}

//=============================================================================
//=============================================================================
UserInfo::~UserInfo()
{
  FUNC_DEBUG;
}

//=============================================================================
//=============================================================================
bool UserInfo::isEmpty()
{
  FUNC_DEBUG;
  return map.isEmpty();
}

//=============================================================================
//=============================================================================
void UserInfo::clear()
{
  FUNC_DEBUG;
  map.clear();
}

//=============================================================================
//=============================================================================
void UserInfo::addUser(QString key, user_struct user)
{
  FUNC_DEBUG;
  USER_MAP::iterator itr;
  itr = map.find(key);
  if (itr == map.end()) {
    map.insert(key, user);
  } else {
    itr->username = user.username;
    itr->password = user.password;
  }
}

//=============================================================================
//=============================================================================
void UserInfo::addUser(QString key, QString userName, QString password)
{
  FUNC_DEBUG;
  user_struct user;
  user.username = userName;
  user.password = password;
  addUser(key, user);
}

//=============================================================================
//=============================================================================
UserInfo::iterator UserInfo::find(QString key)
{
  FUNC_DEBUG;
  return map.find(key);
}

//=============================================================================
//=============================================================================
void UserInfo::removeUser(QString key)
{
  FUNC_DEBUG;
  USER_MAP::iterator itr;
  itr = map.find(key);
  if (itr != map.end()) {
    map.erase(itr);
  }
}

//=============================================================================
//=============================================================================
USER_MAP UserInfo::getUserMap()
{
  FUNC_DEBUG;
  return map;
}


UserInfo* UserInfo::getUserMap(QString iniFile, QStringList keys, QString passphrase)
{
  FUNC_DEBUG;
  INIHelper iniHelper(iniFile);
  UserInfo* info = new UserInfo();
  for (int i = 0; i < keys.size(); i++) {
    iniHelper.readValue("User",keys.at(i));
    info->addUser(keys.at(i)
                 ,iniHelper.readValue("User",keys.at(i))
                 ,Crypt::decrypt(passphrase,iniHelper.readValue("Password",keys.at(i))));
  }
  return info;
}

