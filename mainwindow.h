/*
 * mainwindow.h:
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QSettings>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMenu>
#include <QList>
#include <QFile>
#include <QMap>
#include <QDir>
#include <QCloseEvent>
#include <QtWidgets/QShortcut>

#include "inihelper.h"
#include "puttywidget.h"
#include "actionwrapper.h"
#include "userinfo.h"
#include "docklayout.h"

struct PUTTY_SETTINGS_T {
  QString protocol;
  QString port;
};

typedef QMap<QString, PUTTY_SETTINGS_T> PUTTY_SETTINGS_MAP;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

protected:
  void closeEvent(QCloseEvent* event);

private:
  Ui::MainWindow *ui;
  QStatusBar* statusbar;
  QShortcut* shortcut;
  QSettings* registrySettings;
  INIHelper* iniHelper;
  QMenu* contextMenu;
  QMenu* listBoxContextMenu;
  QMenu* sessionContextMenu;
  QList<ActionWrapper*> actionSessions;
  UserInfo* userinfo;
  PUTTY_SETTINGS_MAP puttySettingsMap;
  PuttyContainer container;
  dockLayout* layouts;
  QString puttyPath;
  QString passphrase;
  int  tabCount;
  bool closeAllTabs;
  bool validForm();
  //void startPutty(QString session, bool newTab, bool standalone = false);
  void startPutty(SessionCfg session, bool newTab, bool standalone = false);
  void loadSessions();
  void loadUsers();
  void loadLayouts();
  void openLayout(QString name);

private slots:
  void on_btnLogin_clicked();
  void on_lwSessions_itemDoubleClicked(QListWidgetItem *item);
  void on_tabWidget_tabCloseRequested(int index);
  void on_tabWidget_currentChanged(int index);
  void on_MainWindow_customContextMenuRequested(const QPoint &pos);
  void on_tabWidget_customContextMenuRequested(const QPoint &pos);
  void on_lwSessions_customContextMenuRequested(const QPoint &pos);
  void on_actionExit_triggered();
  void on_actionClose_All_Tabs_triggered();
  void on_actionNew_Tab_triggered();
  void on_actionChange_Putty_Settings_triggered();
  void on_actionUser_Setup_triggered();
  void on_actionChange_Tab_Title_triggered();
  void on_actionOpen_In_Current_Tab_triggered();
  void on_actionOpen_in_New_Tab_triggered();
  void menuItemSelected(ActionWrapper* action);
  void pwProcessEnded(PuttyWidget* widget);
  void pwChangeTitle(PuttyWidget* widget);  

  void on_actionRefresh_Sessions_triggered();
  void on_actionAbout_triggered();
  void on_txtPassphrase_editingFinished();
  void on_actionOpen_Standalone_triggered();

  void on_actionSave_Current_Layout_triggered();
  void on_actionManage_Layouts_triggered();

  void on_actionOpen_INI_triggered();
};

#endif // MAINWINDOW_H
