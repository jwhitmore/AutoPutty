/*
 * mainwindow.cpp:
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

#include <QDesktopWidget>
#include <QApplication>
#include <QClipboard>
#include "mainwindow.h"
#include "about.h"
#include "usersetup.h"
#include "debug.h"
#include "ui_mainwindow.h"

//=============================================================================
// Sets up registry object and Ini object and adds the sessions from the
// registry into the list widget also grabs putty from resources and copys to
// users %temp% folder
//=============================================================================
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  FUNC_DEBUG;
  ui->setupUi(this);
  statusbar = new QStatusBar(this);
  this->setStatusBar(statusbar);
  tabCount = 1;
  closeAllTabs = false;
  this->setCentralWidget(ui->tabWidget);
  
  userinfo = new UserInfo();

  shortcut = new QShortcut(QKeySequence(Qt::Key_F11),this);
  shortcut->setContext(Qt::ApplicationShortcut);

  registrySettings = new QSettings("HKEY_CURRENT_USER\\Software\\SimonTatham\\PuTTY\\Sessions",QSettings::NativeFormat);
  iniHelper        = new INIHelper(QDir::currentPath() + "\\AutoPutty.ini");

  this->restoreGeometry(iniHelper->readByteArray("Application","geometry"));
  ui->actionCurrent_Tab->setChecked(iniHelper->readValue("Application","defaultTabAction").toInt());
  ui->actionCopy_Password_to_Clipboard->setChecked(iniHelper->readValue("Application","copyPassToClipboard").toInt());
  actionSessions.clear();

  ui->tabWidget->setMovable(true);
  if ( registrySettings != NULL) {
    QStringList list = registrySettings->childGroups();
    QStringList::iterator itr;
    for (itr = list.begin(); itr != list.end(); ++itr) {
      DEBUG << "registrySettings->childGroups()" << (*itr);
      registrySettings->beginGroup((*itr));
      if (registrySettings->value("AltOnly").toInt() != 1) {
        registrySettings->setValue("AltOnly",1);
      }
      registrySettings->endGroup();
      (*itr).replace("%20"," ");
    }

  } else {
    statusbar->showMessage("PuTTY Registry Settings not found.",5000);
  }

  puttyPath = QString("%1\\putty.exe").arg(QDir::tempPath());
  if (!QFile::exists(puttyPath)) {
    QFile::copy(":/executables/putty.exe",puttyPath);
  }
  
  ui->menuView->addAction(ui->dockControl->toggleViewAction());

  // set up context menu
  contextMenu = new QMenu(this);
  listBoxContextMenu = new QMenu(this);
  sessionContextMenu = new QMenu(contextMenu);
  sessionContextMenu->setTitle(tr("Sessions"));
  contextMenu->clear();
  contextMenu->addAction(ui->actionNew_Tab);
  QMenu* tabControlMenu = contextMenu->addMenu("Tab Control");
  tabControlMenu->addAction(ui->actionChange_Tab_Title);
  contextMenu->addSeparator();  
  contextMenu->addMenu(sessionContextMenu);
  contextMenu->addSeparator();
  QMenu* viewMenu = contextMenu->addMenu(tr("View"));
  viewMenu->addAction(ui->dockControl->toggleViewAction());
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->lwSessions->setContextMenuPolicy(Qt::CustomContextMenu);
  loadSessions();
}

//=============================================================================
//=============================================================================
MainWindow::~MainWindow()
{
  FUNC_DEBUG;
  delete ui;
}

//-----------------------------------------------------------------------------
//----------------- Protected -------------------------------------------------

//=============================================================================
// Event is fired when the app closes
//=============================================================================
void MainWindow::closeEvent(QCloseEvent* event)
{
  FUNC_DEBUG;
  if (!container.isEmpty()) {
    switch  (QMessageBox::warning(this, "Open Windows","There are still open windows\nAre you sure you want to exit?","&Yes", "&No"))
    {
      case 1:
        event->ignore();
    }
  }

  iniHelper->writeValue("Application","geometry",this->saveGeometry());
  iniHelper->writeValue("Application","defaultTabAction",(int)ui->actionCurrent_Tab->isChecked());
  iniHelper->writeValue("Application","copyPassToClipboard",(int)ui->actionCopy_Password_to_Clipboard->isChecked());
  if ( iniHelper->readValue("Application","PauseInMsecs").isEmpty() ) {
    iniHelper->writeValue("Application","PauseInMsecs",300);
  }
}

//-----------------------------------------------------------------------------
//----------------- Private ---------------------------------------------------

//=============================================================================
// Validates the user input
//=============================================================================
bool MainWindow::validForm()
{
  FUNC_DEBUG;
  if (ui->txtPassphrase->text().isEmpty()) {
    statusbar->showMessage("Passphrase cannot be blank.",5000);
    return false;
  } else if (ui->lwSessions->selectedItems().count() == 0) {
    statusbar->showMessage("Select a session.",5000);
    return false;
  }
  return true;
}

//=============================================================================
// Starts putty session
//=============================================================================
void MainWindow::startPutty(QString session, bool newTab, bool standalone)
{
  FUNC_DEBUG;
  DEBUG << "session " << session;
  QString protocol = "ssh";
  QString port = "22";
  try
  {
    if (userinfo->isEmpty()) {
      loadUsers();
    }
    UserInfo::iterator user_itr;
    user_itr = userinfo->find(session);
    if (user_itr == userinfo->end()) {
      statusbar->showMessage("User info not found in AutoPutty.ini. Check user setup.",5000);
      return;
    } else {
      if ( user_itr->username.isEmpty() || user_itr->password.isEmpty() ) {
        loadUsers();
        user_itr = userinfo->find(session);
        if (user_itr == userinfo->end()) {
          statusbar->showMessage("User info not found in AutoPutty.ini. Check user setup.",5000);
          return;
        }
      }
    }

    PUTTY_SETTINGS_MAP::Iterator psm_itr;
    psm_itr = puttySettingsMap.find(session);
    if (psm_itr != puttySettingsMap.end()) {
      protocol = psm_itr->protocol;
      port = psm_itr->port;
      //QMessageBox::warning(this,"",QString("%1 %2").arg(protocol).arg(port));
    }
    if (ui->actionCopy_Password_to_Clipboard->isChecked() && protocol != "ssh") {
      QClipboard *clipboard = QApplication::clipboard();
      clipboard->setText(QString("%1%2").arg(user_itr->password).arg("\n"));
    }
    if (standalone) {
      QProcess procPutty;
      QStringList args;
      args << "-load" << session;
      args << "-l" << user_itr->username;
      if (protocol == "ssh") {
        args << "-pw" << user_itr->password;
      }
      args << "-P" << port;
      if (!procPutty.startDetached(puttyPath,args)) {
        statusbar->showMessage("Failed to open putty.exe",5000);
      }
      return;
    }

    PuttyContainer::iterator itr;
    QMainWindow* window = NULL;
    PuttyWidget* lastPutty = NULL;
    if (!newTab) {
      itr = container.find((QMainWindow*)ui->tabWidget->currentWidget());
      if (itr != container.end()) {
        window = itr.key();
        if (!itr.value().isEmpty()) {
          lastPutty = itr.value().last();
        }
      }
      if (ui->tabWidget->currentWidget() != NULL && window == NULL) {
        window = (QMainWindow*)ui->tabWidget->currentWidget();
      }
    }

    if ( window == NULL ) {
      window = new QMainWindow(ui->tabWidget);
      window->setAttribute(Qt::WA_DeleteOnClose,true);
      window->setWindowFlags(Qt::Widget);
      window->setCentralWidget(NULL);
      window->setDockNestingEnabled(true);
    }

    PuttyWidget* putty = new PuttyWidget(window);
    putty->setPause(iniHelper->readValue("Application","PauseInMsecs").toULong());
    window->addDockWidget(Qt::LeftDockWidgetArea,putty);
    if (lastPutty != NULL) {
      window->tabifyDockWidget(lastPutty,putty);
    }

    int idx = 0;
    if (ui->tabWidget->currentIndex() < 0 || newTab) {
      idx = ui->tabWidget->addTab(window,QString("Tab %1").arg(tabCount++));
      ui->tabWidget->setCurrentIndex(idx);
    }

    connect(putty,SIGNAL(processStoppedSignal(PuttyWidget*)),this,SLOT(pwProcessEnded(PuttyWidget*)));
    connect(putty,SIGNAL(changeTitleSignal(PuttyWidget*)),this,SLOT(pwChangeTitle(PuttyWidget*)));
    
    container.add(window,putty);

    if (!putty->startPuttyProcess(puttyPath,session,
                                  user_itr->username,
                                  user_itr->password,
                                  protocol,
                                  port)) {
      window->removeDockWidget(putty);
      pwProcessEnded(putty);
      delete putty;
      statusbar->showMessage("Error starting putty. Check User Setup.",5000);
      return;
    }

  } catch (std::exception &e) {
    DEBUG << "exception " << e.what();
  }
}

//=============================================================================
// Load sessions from registry settings
//=============================================================================
void MainWindow::loadSessions()
{
  FUNC_DEBUG;
  actionSessions.clear();
  ui->menuSessions->clear();
  sessionContextMenu->clear();
  ui->lwSessions->clear();
  listBoxContextMenu->clear();
  puttySettingsMap.clear();

  if ( registrySettings != NULL) {
    QStringList list = registrySettings->childGroups();
    QStringList::iterator itr;
    for (itr = list.begin(); itr != list.end(); itr++) {
      (*itr).replace("%20"," ");
      registrySettings->beginGroup((*itr));
      PUTTY_SETTINGS_T puttySetting;
      puttySetting.port = registrySettings->value("PortNumber").toString();
      puttySetting.protocol = registrySettings->value("Protocol").toString();
      puttySettingsMap.insert((*itr), puttySetting);
      registrySettings->endGroup();
      ActionWrapper* newTab = new ActionWrapper();
      newTab->setText("Open In New Tab");
      newTab->setOptions("Session",(*itr));
      ActionWrapper* curTab = new ActionWrapper();
      curTab->setText("Open In Current Tab");
      curTab->setOptions("Session",(*itr));
      ActionWrapper* standAlone = new ActionWrapper();
      standAlone->setText("Open Standalone");
      standAlone->setOptions("Session",(*itr));
      connect(newTab,SIGNAL(actionTriggered(ActionWrapper*)),this,SLOT(menuItemSelected(ActionWrapper*)));
      connect(curTab,SIGNAL(actionTriggered(ActionWrapper*)),this,SLOT(menuItemSelected(ActionWrapper*)));
      connect(standAlone,SIGNAL(actionTriggered(ActionWrapper*)),this,SLOT(menuItemSelected(ActionWrapper*)));

      QMenu* sessionSubMenu = new QMenu((*itr));
      if (ui->actionCurrent_Tab->isChecked()) {
        sessionSubMenu->addAction(curTab->getAction());
        sessionSubMenu->addAction(newTab->getAction());
        sessionSubMenu->addAction(standAlone->getAction());
        listBoxContextMenu->addAction(ui->actionOpen_In_Current_Tab);
        listBoxContextMenu->addAction(ui->actionOpen_in_New_Tab);
        listBoxContextMenu->addAction(ui->actionOpen_Standalone);
      } else {
        sessionSubMenu->addAction(newTab->getAction());
        sessionSubMenu->addAction(curTab->getAction());
        sessionSubMenu->addAction(standAlone->getAction());
        listBoxContextMenu->addAction(ui->actionOpen_in_New_Tab);
        listBoxContextMenu->addAction(ui->actionOpen_In_Current_Tab);
        listBoxContextMenu->addAction(ui->actionOpen_Standalone);
      }

      sessionContextMenu->addMenu(sessionSubMenu);
      ui->menuSessions->addMenu(sessionSubMenu);
    }
    ui->lwSessions->addItems(list);
    listBoxContextMenu->addSeparator();
    listBoxContextMenu->addAction(ui->actionRefresh_Sessions);
  } else {
    statusbar->showMessage("PuTTY Registry Settings not found.",5000);
  }
}

//=============================================================================
// Load users from ini file
//=============================================================================
void MainWindow::loadUsers()
{
  FUNC_DEBUG;
  if (userinfo != NULL) {
    userinfo->clear();
    QStringList sessions;
    for (int i = 0; i < ui->lwSessions->count(); i++) {
      sessions << ui->lwSessions->item(i)->text();
    }
    userinfo = UserInfo::getUserMap(QDir::currentPath() + "\\AutoPutty.ini", sessions, ui->txtPassphrase->text());
  }
}

//-----------------------------------------------------------------------------
//----------------- Slots -----------------------------------------------------

//=============================================================================
// If menu item Options->User_Setup is checked then it will add users to the
// INI File or else it will read the INI File and open up PuTTY session with
// the INI information
//=============================================================================
void MainWindow::on_btnLogin_clicked()
{
  FUNC_DEBUG;
  if (!validForm()) {
    return;
  }
  try
  {
    // Call this for each item selected in list widget
    for (int i = 0; i < ui->lwSessions->selectedItems().count(); i++) {
      startPutty(ui->lwSessions->selectedItems().at(i)->text(),!ui->actionCurrent_Tab->isChecked());
    }
  } catch (std::exception &e) {
    DEBUG << "exception " << e.what();
  }
}

//=============================================================================
// Trigger for list widget double clicked
//=============================================================================
void MainWindow::on_lwSessions_itemDoubleClicked(QListWidgetItem *item)
{
  FUNC_DEBUG;
  if (!validForm()) {
    return;
  }
  startPutty(item->text(),!ui->actionCurrent_Tab->isChecked());
}

//=============================================================================
// Show Main context menu
//=============================================================================
void MainWindow::on_MainWindow_customContextMenuRequested(const QPoint &pos)
{
  if (contextMenu != NULL) {
    QPoint globalPos = this->mapToGlobal(pos);
    contextMenu->exec(globalPos);
  }
}

//=============================================================================
// Show TabWidget context menu
//=============================================================================
void MainWindow::on_tabWidget_customContextMenuRequested(const QPoint &pos)
{
  if (contextMenu != NULL) {
    QPoint globalPos = ui->tabWidget->mapToGlobal(pos);
    contextMenu->exec(globalPos);
  }
}

//=============================================================================
// Show ListWidget context menu
//=============================================================================
void MainWindow::on_lwSessions_customContextMenuRequested(const QPoint &pos)
{
  if (listBoxContextMenu != NULL) {
    QPoint globalPos = ui->lwSessions->mapToGlobal(pos);
    listBoxContextMenu->exec(globalPos);
  }
}

//=============================================================================
// Triggered when user hits "x" on one of the tabs
// It will kill all child PuTTY processes
//=============================================================================
void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
  FUNC_DEBUG;
  
  QMainWindow* window = (QMainWindow*)ui->tabWidget->widget(index);
  DEBUG << "index " << index;
  PuttyContainer::iterator itr = container.find(window);
  if (itr != container.end()) {
    if (!itr.value().isEmpty()) {
      switch  (QMessageBox::warning(this, "Open Windows","There are still open windows\nAre you sure you want to exit?","&Yes", "&No"))
      {
        case 1:
          return;
      }
      for (int i = 0; i < itr.value().size(); i++) {
        itr.value().at(i)->close();
      }
      itr.value().clear();
      container.remove(window);
    }
  }
  if ( window == (QMainWindow*)ui->tabWidget->widget(index) ) {
    window->close();
    ui->tabWidget->removeTab(index);
  }
}

//=============================================================================
// Triggered when user changes tabs and this tries to focus the PuTTY window
// inside the tab
//=============================================================================
void MainWindow::on_tabWidget_currentChanged(int index)
{
  FUNC_DEBUG;
  
  PuttyContainer::iterator itr = container.find((QMainWindow*)ui->tabWidget->widget(index));
  if (itr != container.end()) {
    if (!itr.value().isEmpty()) {
      itr.value().first()->focus();
    }
  }
}

//=============================================================================
// File->Exit menu item
//=============================================================================
void MainWindow::on_actionExit_triggered()
{
  FUNC_DEBUG;
  this->close();
}

//=============================================================================
// File->Close_All_Tabs menu item
//=============================================================================
void MainWindow::on_actionClose_All_Tabs_triggered()
{
  FUNC_DEBUG;
  try
  {
    if (!container.isEmpty()) {
      switch  (QMessageBox::warning(this, "Open Windows","There are still open windows\nAre you sure you want to exit?","&Yes", "&No"))
      {
        case 1:
          return;
      }      
    }
    
    PuttyContainer::iterator itr;
    for (itr = container.begin(); itr != container.end(); itr++) {
      for (int i = 0; i < itr.value().size(); i++) {
        itr.value().at(i)->close();
      }
    }
  } catch (std::exception &e) {
    DEBUG << "exception " << e.what();
  }
}

//=============================================================================
// File->New_Tab menu item
//=============================================================================
void MainWindow::on_actionNew_Tab_triggered()
{
  FUNC_DEBUG;
  QMainWindow* window = NULL;
  window = new QMainWindow(ui->tabWidget);
  window->setAttribute(Qt::WA_DeleteOnClose,true);
  window->setWindowFlags(Qt::Widget);
  window->setCentralWidget(NULL);
  window->setDockNestingEnabled(true);

  int idx = ui->tabWidget->addTab(window,QString("Tab %1").arg(tabCount++));
  ui->tabWidget->setCurrentIndex(idx);
}

//=============================================================================
// Options->Change_Putty_Settings menu item
//=============================================================================
void MainWindow::on_actionChange_Putty_Settings_triggered()
{
  FUNC_DEBUG;
  try
  {
    QProcess procPutty;
    if (!procPutty.startDetached(puttyPath)) {
      statusbar->showMessage("Failed to open putty.exe",5000);
    }
  } catch (std::exception &e) {
    DEBUG << "exception " << e.what();
    statusbar->showMessage("Exception opening putty.exe",5000);
  }
}

//=============================================================================
// Options->User_Setup menu item
//=============================================================================
void MainWindow::on_actionUser_Setup_triggered()
{
  FUNC_DEBUG;
  UserSetup usersetup;
  usersetup.setPassphrase(ui->txtPassphrase->text());
  usersetup.exec();
  loadUsers();
}

//=============================================================================
// Options->Refresh_Sessions menu item
//=============================================================================
void MainWindow::on_actionRefresh_Sessions_triggered()
{
  loadSessions();
  if (!ui->txtPassphrase->text().isEmpty()) {
    loadUsers();
  }
}

//=============================================================================
// Options->Tab Control->Change_Tab_Title menu item
//=============================================================================
void MainWindow::on_actionChange_Tab_Title_triggered()
{
  FUNC_DEBUG;
  bool ok;
  if (ui->tabWidget->currentIndex() < 0) { return; }
  QString text = QInputDialog::getText(this,tr("Change Tab Title"),
                                       tr("New title:"), QLineEdit::Normal,
                                       ui->tabWidget->tabText(ui->tabWidget->currentIndex()), &ok);
  if (ok && !text.isEmpty()) {
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),text);
  }
}


//=============================================================================
// Sessions->SessionName->Open_In_Current_Tab menu item
//=============================================================================
void MainWindow::on_actionOpen_In_Current_Tab_triggered()
{
  FUNC_DEBUG;
  if (!validForm()) {
    return;
  }
  for (int i = 0; i < ui->lwSessions->selectedItems().count(); i++) {
    startPutty(ui->lwSessions->selectedItems().at(i)->text(),false);
  }
}

//=============================================================================
// Sessions->SessionName->Open_In_New_Tab menu item
//=============================================================================
void MainWindow::on_actionOpen_in_New_Tab_triggered()
{
  FUNC_DEBUG;
  if (!validForm()) {
    return;
  }
  for (int i = 0; i < ui->lwSessions->selectedItems().count(); i++) {
    startPutty(ui->lwSessions->selectedItems().at(i)->text(),true);
  }
}

//=============================================================================
// Sessions->SessionName->Open_Standalone menu item
//=============================================================================
void MainWindow::on_actionOpen_Standalone_triggered()
{
  FUNC_DEBUG;
  if (!validForm()) {
    return;
  }
  for (int i = 0; i < ui->lwSessions->selectedItems().count(); i++) {
    startPutty(ui->lwSessions->selectedItems().at(i)->text(),false,true);
  }
}

//=============================================================================
// Help->About AutoPutty menu item
//=============================================================================
void MainWindow::on_actionAbout_triggered()
{
  About about;
  about.exec();
}


//=============================================================================
// On leave of passphrase, reload userinfo when leaving field so the passwords
// and usernames stay updated correctly
//=============================================================================
void MainWindow::on_txtPassphrase_editingFinished()
{
  FUNC_DEBUG;
  if (passphrase == ui->txtPassphrase->text()) { return; }
  if (!ui->txtPassphrase->text().isEmpty()) {
    loadUsers();
  } else {
    if (userinfo != NULL) {
      userinfo->clear();
    }
  }
  passphrase = ui->txtPassphrase->text();
}

//=============================================================================
// Dynamic menu handler
//=============================================================================
void MainWindow::menuItemSelected(ActionWrapper *actionWrapper)
{
  FUNC_DEBUG;
  if (ui->txtPassphrase->text().isEmpty()) {
    statusbar->showMessage("Passphrase cannot be blank.",5000);
    return;
  }

  if (actionWrapper->getText() == "Open In New Tab") {
    QString session = actionWrapper->getOption("Session");
    startPutty(session,true);
  } else if (actionWrapper->getText() == "Open In Current Tab") {
    QString session = actionWrapper->getOption("Session");
    startPutty(session,false);
  } else if (actionWrapper->getText() == "Open Standalone"){
    QString session = actionWrapper->getOption("Session");
    startPutty(session,false,true);
  } else {
    startPutty(actionWrapper->getText(),!ui->actionCurrent_Tab->isChecked());
  }
}

//=============================================================================
// Slot to catch when putty has exited and close the tab automatically if all
// child putty processes are out of parent tab
//=============================================================================
void MainWindow::pwProcessEnded(PuttyWidget* widget)
{
  FUNC_DEBUG;
  PuttyContainer::iterator itr = container.find(widget);
  if ( itr != container.end()) {
    container.removePutty(widget);
    itr.key()->removeDockWidget(widget);
    widget->close();
    if (!itr.value().isEmpty()) {
      itr.value().first()->focus();
    } else {
      for (int i = 0; i < ui->tabWidget->count(); i++) {
        if (ui->tabWidget->widget(i) == itr.key()) {
          itr.key()->close();
          container.remove(itr.key());
          ui->tabWidget->removeTab(i);
          break;
        }
      }
    }
  }
}

//=============================================================================
// Slot for changing the PuttyWidget window title
//=============================================================================
void MainWindow::pwChangeTitle(PuttyWidget* widget)
{
  FUNC_DEBUG;
  bool ok = false;
  QString text = QInputDialog::getText(this,tr("Change Title"),
                                       tr("New title:"), QLineEdit::Normal,
                                       widget->getTitle(), &ok);
  if (ok && !text.isEmpty()) {
    widget->setTitle(text);
  }
}
