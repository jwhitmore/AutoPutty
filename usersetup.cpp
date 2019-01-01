/*
 * usersetup.cpp:
 * AutoPutty: Auto-logon/Tabbed PuTTy
 * Copyright (c) 2012-2019 Justin Whitmore
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

#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QCloseEvent>

#include "crypt.h"
#include "usersetup.h"
#include "debug.h"

#include "ui_usersetup.h"

#define COL_SESSION  0
#define COL_USERNAME 1
#define COL_PASSWORD 2
#define COL_COUNT    3

//=============================================================================
//=============================================================================
UserSetup::UserSetup(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::UserSetup)
{
  FUNC_DEBUG;
  ui->setupUi(this);
  this->iniHelper = new INIHelper(QDir::currentPath() + "\\AutoPutty.ini");
  this->userInfo = new UserInfo();
  _showPasswords = false;
  _changes = false;
  ui->btnClose->setEnabled(false);
  ui->btnSave->setEnabled(false);
  ui->btnUpdate->setEnabled(false);
  ui->txtPassword->setEnabled(false);
  ui->txtUserName->setEnabled(false);
  ui->cbShowPasswords->setEnabled(false);
}

//=============================================================================
//=============================================================================
UserSetup::~UserSetup()
{
  FUNC_DEBUG;
  delete iniHelper;
  delete userInfo;
  delete ui;
}

//=============================================================================
//=============================================================================
void UserSetup::closeEvent(QCloseEvent* event)
{
  FUNC_DEBUG;
  if (_changes) {
    switch  (QMessageBox::warning(this, "Close","Are you sure you want to close this window without saving changes?","&Yes", "&No"))
    {
      case 1:
        event->ignore();
    }
  }
}

//=============================================================================
//=============================================================================
void UserSetup::setPassphrase(QString passphrase)
{
  FUNC_DEBUG;
  ui->txtPassphrase->setText(passphrase);
  if (!passphrase.isEmpty()) {
    loadSessions();
  }
}

//=============================================================================
//=============================================================================
void UserSetup::loadSessions()
{
  FUNC_DEBUG;
  if (ui->txtPassphrase->text().isEmpty()) {
    QMessageBox::warning(this,tr("Error"),tr("Passphrase is blank"));
    return;
  }
  this->userInfo->clear();
  QSettings reg("HKEY_CURRENT_USER\\Software\\SimonTatham\\PuTTY\\Sessions",QSettings::NativeFormat);
  QStringList list = reg.childGroups();
  QStringList::iterator itr;
  for (itr = list.begin(); itr != list.end(); ++itr) {
    reg.beginGroup((*itr));
    reg.endGroup();

    (*itr).replace("%20"," ");
    user_struct user;
    user.username = this->iniHelper->readValue("User",(*itr));
    user.password = Crypt::decrypt(ui->txtPassphrase->text(),this->iniHelper->readValue("Password",(*itr)));
    this->userInfo->addUser((*itr),user);

  }
  if (this->userInfo->getUserMap().size() > 0) {
    fillTable((*this->userInfo));
    ui->btnClose->setEnabled(true);
    ui->btnSave->setEnabled(true);
    ui->btnUpdate->setEnabled(true);
    ui->txtPassword->setEnabled(true);
    ui->txtUserName->setEnabled(true);
    ui->cbShowPasswords->setEnabled(true);
  }
}

//=============================================================================
//=============================================================================
void UserSetup::on_txtPassphrase_returnPressed()
{
  FUNC_DEBUG;
  loadSessions();
}

//=============================================================================
//=============================================================================
void UserSetup::on_btnUpdate_clicked()
{
  FUNC_DEBUG;
  if (ui->txtPassphrase->text().isEmpty() || ui->txtUserName->text().isEmpty() || ui->txtPassword->text().isEmpty()) {
    QMessageBox::warning(this,tr("Error"),tr("Text fields cannot be empty"));
    return;
  } else if (ui->tblUser->selectedItems().count() <= 0) {
    QMessageBox::warning(this,tr("Error"),tr("No rows selected"));
    return;
  }

  QModelIndexList rows = ui->tblUser->selectionModel()->selectedRows();
  rows.at(0).row();
  for (int i = 0; i < rows.count(); i++) {
    UserInfo::iterator itr = this->userInfo->find(ui->tblUser->item(rows.at(i).row(),COL_SESSION )->text());
    if (itr != this->userInfo->end()) {
      itr.value().password = ui->txtPassword->text();
      itr.value().username = ui->txtUserName->text();
    }
  }
  fillTable((*this->userInfo));
  _changes = true;

}

//=============================================================================
//=============================================================================
void UserSetup::on_cbShowPasswords_clicked(bool checked)
{
  FUNC_DEBUG;
  this->_showPasswords = checked;

  for (int i = 0; i < ui->tblUser->rowCount(); i++) {
    QTableWidgetItem* password = ui->tblUser->item(i,COL_PASSWORD);

    UserInfo::iterator itr_map = this->userInfo->find(ui->tblUser->item(i,COL_SESSION)->text());
    if (itr_map != this->userInfo->end()) {
      password->setText(this->_showPasswords ? itr_map.value().password : Crypt::encrypt(ui->txtPassphrase->text(),itr_map.value().password));
    }
  }
  ui->tblUser->resizeColumnsToContents();
  ui->tblUser->resizeRowsToContents();
}

//=============================================================================
//=============================================================================
void UserSetup::fillTable(UserInfo info)
{
  FUNC_DEBUG;
  ui->tblUser->setRowCount(0);

  UserInfo::iterator itr_map;
  for(itr_map = info.begin(); itr_map != info.end(); itr_map++) {
    const int currentRow = ui->tblUser->rowCount();
    ui->tblUser->setRowCount(currentRow + 1);
    for (int cidx = 0; cidx < COL_COUNT; cidx++) {
      QString label = "";
      if (cidx == 0) {
        label = itr_map.key();
      } else if (cidx == 1) {
        label = itr_map.value().username;
      } else if (cidx == 2) {
        if (this->_showPasswords) {
          label = itr_map.value().password;
        } else {
          label = Crypt::encrypt(ui->txtPassphrase->text(),itr_map.value().password);
        }
      }

      QTableWidgetItem* item = new QTableWidgetItem(label);
      ui->tblUser->setItem(currentRow,cidx,item);

    }
  }
  ui->tblUser->resizeColumnsToContents();
  ui->tblUser->resizeRowsToContents();
}

//=============================================================================
//=============================================================================
void UserSetup::on_btnSave_clicked()
{
  FUNC_DEBUG;
  if (ui->txtPassphrase->text().isEmpty()) {
    QMessageBox::warning(this,"Error","Passphrase cannot be empyt");
    return;
  }
  if ( !_changes ) { return; }
  switch  (QMessageBox::warning(this, "Save Changes","Are you sure you want to save changes?","&Yes", "&No"))
  {
    case 1:
      return;
  }
  UserInfo::iterator itr;
  for (itr = userInfo->begin(); itr != userInfo->end(); itr++) {
    iniHelper->writeValue("User",itr.key(),itr.value().username);
    iniHelper->writeValue("Password",itr.key(),Crypt::encrypt(ui->txtPassphrase->text(),itr.value().password));
  }
  loadSessions();
  _changes = false;
}

//=============================================================================
//=============================================================================
void UserSetup::on_btnClose_clicked()
{
  FUNC_DEBUG;

  this->close();
}
