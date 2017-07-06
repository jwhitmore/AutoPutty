/*
 * actionwrapper.cpp:
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

#include "debug.h"
#include "actionwrapper.h"

//=============================================================================
// Wrapper around QActions
// wrote this for putting dynamic actions into a menu and getting the QAction
// object when that action was triggered
//=============================================================================
ActionWrapper::ActionWrapper(QObject *parent) :
  QObject(parent)
{
  FUNC_DEBUG;
  action = new QAction(parent);
  connect(action,SIGNAL(triggered()),this,SLOT(gotTriggered()));
}

//=============================================================================
//=============================================================================
ActionWrapper::~ActionWrapper()
{
  FUNC_DEBUG;
  if (action == NULL) {
    delete action;
  }
}

//=============================================================================
//=============================================================================
QAction* ActionWrapper::getAction()
{
  FUNC_DEBUG;
  return action;
}

//=============================================================================
//=============================================================================
QString ActionWrapper::getText()
{
  FUNC_DEBUG;
  if (action != NULL) {
    return action->text();
  } else {
    return QString();
  }
}

//=============================================================================
//=============================================================================
void ActionWrapper::setText(QString text)
{
  FUNC_DEBUG;
  if (action != NULL) {
    action->setText(text);
  }
}

//=============================================================================
//=============================================================================
void ActionWrapper::setOptions(QString name, QString value)
{
  FUNC_DEBUG;

  QMap<QString,QString>::iterator itr = options.find(name);
  if (itr == options.end()) {
    options.insert(name,value);
  } else {
    itr.value() = value;
  }
}

//=============================================================================
//=============================================================================
QString ActionWrapper::getOption(QString name)
{
  FUNC_DEBUG;

  if (options.find(name) == options.end()) return "";
  else return options.find(name).value();
}

//=============================================================================
//=============================================================================
void ActionWrapper::gotTriggered()
{
  FUNC_DEBUG;
  emit actionTriggered(this);
}
