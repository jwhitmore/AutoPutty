/*
 * usersetup.h:
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

#ifndef USERSETUP_H
#define USERSETUP_H

#include <QDialog>

#include "userinfo.h"
#include "inihelper.h"


namespace Ui {
class UserSetup;
}

class UserSetup : public QDialog
{
  Q_OBJECT

public:
  explicit UserSetup(QWidget *parent = 0);
  ~UserSetup();
  void setPassphrase(QString passphrase);

protected:
  void closeEvent(QCloseEvent* event);
  
private slots:
  void on_txtPassphrase_returnPressed();

  void on_btnUpdate_clicked();

  void on_cbShowPasswords_clicked(bool checked);

  void on_btnSave_clicked();

  void on_btnClose_clicked();

private:
  Ui::UserSetup *ui;
  INIHelper* iniHelper;
  UserInfo* userInfo;
  bool _showPasswords;
  bool _changes;

  void loadSessions();
  void fillTable(const UserInfo info);
};

#endif // USERSETUP_H
