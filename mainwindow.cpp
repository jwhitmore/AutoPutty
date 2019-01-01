/*
 * mainwindow.cpp:
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

#include <QDesktopWidget>
#include <QApplication>
#include <QClipboard>
#include "mainwindow.h"
#include "about.h"
#include "usersetup.h"
#include "managelayouts.h"
#include "debug.h"
#include "ui_mainwindow.h"

int layoutID = 0;
int puttyID = 0;

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

  // Set our OrgName and AppName (so QSettings is consistent)
  QCoreApplication::setOrganizationName("autoputty");
  QCoreApplication::setApplicationName("autoputty");

  layouts = new dockLayout(QCoreApplication::organizationName(), QCoreApplication::applicationName());

  ui->tabWidget->setMovable(true);
  if ( registrySettings != nullptr) {
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
  loadLayouts();
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
  iniHelper->writeValue("Application","defaultTabAction",static_cast<int>(ui->actionCurrent_Tab->isChecked()));
  iniHelper->writeValue("Application","copyPassToClipboard",static_cast<int>(ui->actionCopy_Password_to_Clipboard->isChecked()));
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
void MainWindow::startPutty(SessionCfg session, bool newTab, bool standalone)
{
  FUNC_DEBUG;
  DEBUG << "session " << session.sessionName;
  QString protocol = "ssh";
  QString port = "22";
  try
  {
    if (userinfo->isEmpty()) {
      loadUsers();
    }
    UserInfo::iterator user_itr;
    user_itr = userinfo->find(session.sessionName);
    if (user_itr == userinfo->end()) {
      statusbar->showMessage("User info not found in AutoPutty.ini. Check user setup.",5000);
      return;
    } else {
      if ( user_itr->username.isEmpty() || user_itr->password.isEmpty() ) {
        loadUsers();
        user_itr = userinfo->find(session.sessionName);
        if (user_itr == userinfo->end()) {
          statusbar->showMessage("User info not found in AutoPutty.ini. Check user setup.",5000);
          return;
        }
      }
    }

    PUTTY_SETTINGS_MAP::Iterator psm_itr;
    psm_itr = puttySettingsMap.find(session.sessionName);
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
      args << "-load" << session.sessionName;
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
    QMainWindow* window = nullptr;
    PuttyWidget* lastPutty = nullptr;
    if (!newTab) {
      itr = container.find(static_cast<QMainWindow*>(ui->tabWidget->currentWidget()));
      if (itr != container.end()) {
        window = itr.key();        
        if (!itr.value().isEmpty()) {
          lastPutty = itr.value().last();
        }
      }
      if (ui->tabWidget->currentWidget() != nullptr && window == nullptr) {
        window = static_cast<QMainWindow*>(ui->tabWidget->currentWidget());
      }
    }

    if ( window == nullptr ) {
      window = new QMainWindow(ui->tabWidget);
      window->setAttribute(Qt::WA_DeleteOnClose,true);
      window->setWindowFlags(Qt::Widget);
      window->setCentralWidget(nullptr);
      window->setDockNestingEnabled(true);
      window->setObjectName(session.layoutObjectName);
      window->setProperty("layoutName", QString("UnsavedLayout%1").arg(layoutID++));
    }

    PuttyWidget* putty = new PuttyWidget(window);
    putty->setObjectName(session.sessionObjectName);

    // probably don't need since we're clearing all sessions and re-adding (as opposed to trying to manage which is which in the config)
    //putty->setSessionId(session.sessionConfigId);

    putty->setPause(iniHelper->readValue("Application","PauseInMsecs").toULong());
    window->addDockWidget(Qt::LeftDockWidgetArea,putty);
    if (lastPutty != nullptr) {
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

    if (!putty->startPuttyProcess(puttyPath,session.sessionName,
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

  if ( registrySettings != nullptr) {
    QStringList list = registrySettings->childGroups();
    QStringList::iterator itr;
    for (itr = list.begin(); itr != list.end(); itr++) {
      registrySettings->beginGroup((*itr));
      PUTTY_SETTINGS_T puttySetting;
      puttySetting.port = registrySettings->value("PortNumber").toString();
      puttySetting.protocol = registrySettings->value("Protocol").toString();
      puttySettingsMap.insert((*itr), puttySetting);
      registrySettings->endGroup();
      (*itr).replace("%20"," ");      
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
  if (userinfo != nullptr) {
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

      SessionCfg scfg;
      scfg.sessionName = ui->lwSessions->selectedItems().at(i)->text();
      scfg.sessionObjectName = QString("UnsavedSession%1").arg(puttyID++);

      startPutty(scfg, !ui->actionCurrent_Tab->isChecked());
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

  SessionCfg scfg;
  scfg.sessionName = item->text();
  scfg.sessionObjectName = QString("UnsavedSession%1").arg(puttyID++);

  startPutty(scfg, !ui->actionCurrent_Tab->isChecked());
}

//=============================================================================
// Show Main context menu
//=============================================================================
void MainWindow::on_MainWindow_customContextMenuRequested(const QPoint &pos)
{
  if (contextMenu != nullptr) {
    QPoint globalPos = this->mapToGlobal(pos);
    contextMenu->exec(globalPos);
  }
}

//=============================================================================
// Show TabWidget context menu
//=============================================================================
void MainWindow::on_tabWidget_customContextMenuRequested(const QPoint &pos)
{
  if (contextMenu != nullptr) {
    QPoint globalPos = ui->tabWidget->mapToGlobal(pos);
    contextMenu->exec(globalPos);
  }
}

//=============================================================================
// Show ListWidget context menu
//=============================================================================
void MainWindow::on_lwSessions_customContextMenuRequested(const QPoint &pos)
{
  if (listBoxContextMenu != nullptr) {
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
  
  QMainWindow* window = static_cast<QMainWindow*>(ui->tabWidget->widget(index));
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
  if ( window == static_cast<QMainWindow*>(ui->tabWidget->widget(index)) ) {
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
  
  PuttyContainer::iterator itr = container.find(static_cast<QMainWindow*>(ui->tabWidget->widget(index)));
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
  QMainWindow* window = nullptr;
  window = new QMainWindow(ui->tabWidget);
  window->setAttribute(Qt::WA_DeleteOnClose,true);
  window->setWindowFlags(Qt::Widget);
  window->setCentralWidget(nullptr);
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

    SessionCfg scfg;
    scfg.sessionName = ui->lwSessions->selectedItems().at(i)->text();
    scfg.sessionObjectName = QString("UnsavedSession%1").arg(puttyID++);

    startPutty(scfg ,false);
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

    SessionCfg scfg;
    scfg.sessionName = ui->lwSessions->selectedItems().at(i)->text();
    scfg.sessionObjectName = QString("UnsavedSession%1").arg(puttyID++);

    startPutty(scfg, true);
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

    SessionCfg scfg;
    scfg.sessionName = ui->lwSessions->selectedItems().at(i)->text();
    scfg.sessionObjectName = QString("UnsavedSession%1").arg(puttyID++);

    startPutty(scfg,false,true);
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
    if (userinfo != nullptr) {
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

  DEBUG << "getOption Session: " << actionWrapper->getOption("Session");
  DEBUG << "getOption Layout : " << actionWrapper->getOption("Layout");


  if (!actionWrapper->getOption("Session").isEmpty()) {

    SessionCfg scfg;
    scfg.sessionName = actionWrapper->getOption("Session");
    scfg.sessionObjectName = QString("UnsavedSession%1").arg(puttyID++);

    if (actionWrapper->getText() == "Open In New Tab") {
      startPutty(scfg,true);
    } else if (actionWrapper->getText() == "Open In Current Tab") {
      startPutty(scfg,false);
    } else if (actionWrapper->getText() == "Open Standalone"){
      startPutty(scfg,false,true);
    } else {
      scfg.sessionName = actionWrapper->getText();
      startPutty(scfg,!ui->actionCurrent_Tab->isChecked());
    }
  } else if (!actionWrapper->getOption("Layout").isEmpty()) {
    openLayout(actionWrapper->getOption("Layout"));
  } else {
    DEBUG << "wut?";
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

//=============================================================================
//=============================================================================
void MainWindow::on_actionSave_Current_Layout_triggered()
{
  FUNC_DEBUG;

  if ( ui->tabWidget->currentIndex() < 0 ) {
    statusbar->showMessage("Error: No sessions are open", 5000);
    return;
  }

  PuttyContainer::iterator itr;
  QMainWindow* window = nullptr;

  itr = container.find(static_cast<QMainWindow*>(ui->tabWidget->currentWidget()));
  if (itr != container.end()) {
    window = itr.key();
  }
  if (ui->tabWidget->currentWidget() != nullptr && window == nullptr) {
    window = static_cast<QMainWindow*>(ui->tabWidget->currentWidget());
  }

  QString layoutName;
  bool newLayout = false;

  DEBUG << "window->property - layoutName - value: [" << window->property("layoutName").toString() << "]";

  if ( window->property("layoutName").toString().contains("UnsavedLayout")) {
    bool ok;
    layoutName = QInputDialog::getText(this, "Unsaved Layout", "Enter a new name for your layout:", QLineEdit::Normal, "", &ok);
    if (ok && !layoutName.isEmpty()) {
      window->setProperty("layoutName", layoutName);
      newLayout = true;
    } else {
      statusbar->showMessage("Layout was not saved. No layout name was specified.", 5000);
      return;
    }
  } else {
    layoutName = window->property("layoutName").toString();
  }

  DEBUG << "layoutName: " << layoutName;
  DEBUG << "windowName: " << window->property("layoutName").toString();

  QSettings settings;
  settings.setValue(QString("layout/%1/geometry").arg(layoutName), window->saveGeometry());
  settings.setValue(QString("layout/%1/windowState").arg(layoutName), window->saveState());
  settings.setValue(QString("layout/%1/objectName").arg(layoutName), window->objectName());

  itr = container.find(static_cast<QMainWindow*>(ui->tabWidget->currentWidget()));
  if (itr != container.end()) {
    DEBUG << "-- " << itr.key()->property("layoutName").toString();

    // Clear existing config entries
    settings.remove(QString("layout/%1/sessions").arg(layoutName));

    for (int i = 0; i < itr.value().size(); i++) {

      PuttyWidget* pw = itr.value().at(i);

      settings.setValue(QString("layout/%1/sessions/%2/sessionName").arg(layoutName).arg(i+1), pw->getSession());
      settings.setValue(QString("layout/%1/sessions/%2/objectName").arg(layoutName).arg(i+1), pw->objectName());

      DEBUG << "---- " << (i+1) << "/" << itr.value().size() << ": " << pw->getSession();
    }
  }

  ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), layoutName);

  if ( newLayout ) {
    loadLayouts();
  }
}


//=============================================================================
//=============================================================================
void MainWindow::loadLayouts()
{
  FUNC_DEBUG;

  if ( layouts != nullptr) {

    QStringList list = layouts->getLayoutNames();
    QStringList::iterator itr;

    QList<QAction*> actions = ui->menuLayout->actions();
    for(int i = 3; i < actions.count(); i++ ) {
      ui->menuLayout->removeAction(actions.at(i));
    }

    for (itr = list.begin(); itr != list.end(); itr++) {

      //registrySettings->beginGroup((*itr));

      DEBUG << "itr [" << (*itr) << "]";

      (*itr).replace("%20"," ");
      ActionWrapper* openLayout = new ActionWrapper();
      openLayout->setText((*itr));
      openLayout->setOptions("Layout",(*itr));
      connect(openLayout,SIGNAL(actionTriggered(ActionWrapper*)),this,SLOT(menuItemSelected(ActionWrapper*)));

      ui->menuLayout->addAction(openLayout->getAction());
    }

  } else {
    statusbar->showMessage("PuTTY Registry Settings not found.",5000);
  }
}

//=============================================================================
//=============================================================================
void MainWindow::openLayout(QString name)
{
  FUNC_DEBUG;

  QVector<SessionCfg> sessions = layouts->getLayoutSessions(name);
  QString objName = layouts->getLayoutObjectName(name);

  DEBUG << "Layout " << name << " (object: " << objName << ") - " << sessions.count() << " sessions.";

  for ( int i = 0; i < sessions.count(); i++ ) {
    DEBUG << "Starting Session " << (i+1) << ": " << sessions.at(i).sessionName << " -- " << sessions.at(i).sessionObjectName;
    if ( i == 0 ) {
      this->startPutty(sessions.at(i), true, false);
    } else {
      this->startPutty(sessions.at(i), false, false);
    }
  }

  PuttyContainer::iterator itr;
  QMainWindow* window = nullptr;
  itr = container.find(static_cast<QMainWindow*>(ui->tabWidget->currentWidget()));
  if (itr != container.end()) {
    window = itr.key();
  }
  if (ui->tabWidget->currentWidget() != nullptr && window == nullptr) {
    window = static_cast<QMainWindow*>(ui->tabWidget->currentWidget());
    //window = (QMainWindow*)ui->tabWidget->currentWidget();
  }

  window->setObjectName(objName);
  window->setProperty("layoutName", name);
  ui->tabWidget->setTabText( ui->tabWidget->currentIndex(), name);

  Thread::msleep(500);

  window->restoreGeometry(layouts->getLayoutGeometry(name));
  window->restoreState(layouts->getLayoutState(name));
}

//=============================================================================
//=============================================================================
void MainWindow::on_actionManage_Layouts_triggered()
{
  FUNC_DEBUG;

  if ( layouts != nullptr) {
    manageLayouts* manager = new manageLayouts(this);
    manager->setLayouts(layouts);
    manager->exec();

    loadLayouts();


  } else {
    DEBUG << "dockLayout layouts is NULL";
  }

}

//=============================================================================
//=============================================================================
void MainWindow::on_actionOpen_INI_triggered()
{

}
