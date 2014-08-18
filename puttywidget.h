/*
 * puttywidget.h:
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

#ifndef PUTTYWIDGET_H
#define PUTTYWIDGET_H

#include <QDockWidget>
#include <QProcess>
#include <QThread>
#include <QDebug>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QMainWindow>
#include <QMap>
#include <QAbstractButton>

#include <stdexcept>

#include <windows.h>

class Thread : private QThread
{
public:
  static void sleep(unsigned long secs)
  {
    QThread::sleep(secs); // 1 = 1 sec
  }
  static void msleep(unsigned long msecs)
  {
    QThread::msleep(msecs); // 1,000 = 1 sec
  }
  static void usleep(unsigned long usecs)
  {
    QThread::usleep(usecs); // 1,000,000 = 1 sec
  }
};

class PuttyWidget : public QDockWidget
{
  Q_OBJECT
public:
  explicit PuttyWidget(QWidget *parent = 0);
  ~PuttyWidget();
  bool startPuttyProcess(QString puttyPath, QString session, QString username, QString password);
  void setTitle(QString title);
  void setPause(unsigned long msec);
  QString getTitle();
  bool isRunning();
  bool kill();
  bool focus();

protected:
  bool eventFilter(QObject *object, QEvent *event);
  void resizeEvent(QResizeEvent* event);
  void keyPressEvent(QKeyEvent * event);

private:  
  QProcess* procPutty;
  QMenu* menuContext;
  HWND puttyHandle;
  HWND puttyParent;
  unsigned long msecs;
  bool attached;
  QAbstractButton* btnFloat;

  QAction* actionChangeTitle;
  bool addProcToWidget();
  HWND findPuttyWindow(QProcess* process);

private slots:
  void processStateChanged(QProcess::ProcessState state);
  void changeTitle_triggered();
  void customContextMenuRequested(const QPoint &pos);
  void topLevelChanged(bool topLevel);

signals:
  void processStoppedSignal(PuttyWidget* widget);
  void changeTitleSignal(PuttyWidget* widget);
};

typedef  QMap< QMainWindow*,QList<PuttyWidget*> > PUTTY_MAP;

class PuttyContainer
{
public:
  PuttyContainer()  { } // Constructor
  ~PuttyContainer() { } // Deconstructor
  
  typedef PUTTY_MAP::iterator iterator;
  typedef const PUTTY_MAP::const_iterator const_iterator;
  iterator begin() { return map.begin(); }
  iterator end()   { return map.end();   }
  
  void add(QMainWindow* window, PuttyWidget* widget);
  void removePutty(PuttyWidget* widget);
  void remove(QMainWindow* window);
  bool isEmpty();
  PuttyContainer::iterator find(PuttyWidget* widget);
  PuttyContainer::iterator find(QMainWindow* window);

private:
  PUTTY_MAP map;
};

#endif // PUTTYWIDGET_H
