/*
 * puttywidget.cpp:
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

#include "debug.h"
#include "puttywidget.h"

/*****************************************************************************
 * START OF PuttyContainer
 *****************************************************************************/

//=============================================================================
//=============================================================================
void PuttyContainer::add(QMainWindow* window, PuttyWidget* widget)
{
  PUTTY_MAP::iterator itr;
  itr = map.find(window);
  if (itr != map.end()) {
    PuttyContainer::iterator p_itr = find(widget);
    if (p_itr == end()) {
      itr.value() << widget;
    }
  } else {
    QList<PuttyWidget*> list;
    list << widget;
    map.insert(window,list);
  }
}

//=============================================================================
//=============================================================================
void PuttyContainer::removePutty(PuttyWidget* widget)
{
  PuttyContainer::iterator itr = find(widget);
  if (itr != end()) {
    itr.value().removeOne(widget);
  }
}

//=============================================================================
//=============================================================================
void PuttyContainer::remove(QMainWindow* window) 
{
  PuttyContainer::iterator itr = find(window);
  if (itr != end()) {    
    map.erase(itr);
  }
}

//=============================================================================
//=============================================================================
PuttyContainer::iterator PuttyContainer::find(PuttyWidget* widget)
{
  PuttyContainer::iterator itr;
  for (itr = map.begin(); itr != map.end(); itr++) {
    for (int i = 0; i < itr.value().size(); i++) {
      if (itr.value().at(i) == widget) { return itr; }
    }
  }
  return end();
}

//=============================================================================
//=============================================================================
PuttyContainer::iterator PuttyContainer::find(QMainWindow* window)
{
  return map.find(window);
}

//=============================================================================
//=============================================================================
bool PuttyContainer::isEmpty()
{
  return map.isEmpty();
}

/*****************************************************************************
 * END OF PuttyContainer
 *****************************************************************************/

//=============================================================================
//=============================================================================
PuttyWidget::PuttyWidget(QWidget *parent) :
  QDockWidget(parent)
{
  FUNC_DEBUG;
  procPutty = NULL;
  attached = false;
  msecs = 300;
  widget = new QWidget(this);
  this->setAttribute(Qt::WA_DeleteOnClose,true);
  widget->setAttribute(Qt::WA_DeleteOnClose,true);
  this->setWidget(widget);
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  this->setFocusPolicy(Qt::ClickFocus);
  this->resize(parent->size());
  actionChangeTitle = new QAction(tr("Change Title"),this);
  menuContext = new QMenu(this);

  menuContext->addAction(actionChangeTitle);
  connect(actionChangeTitle,SIGNAL(triggered()),this,SLOT(changeTitle_triggered()));
  connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuRequested(QPoint)));

}

//=============================================================================
//=============================================================================
PuttyWidget::~PuttyWidget()
{
  FUNC_DEBUG;
  if (procPutty != NULL) {
    if (isRunning()) {
      kill();
    }
    delete procPutty;
  }
  if (widget != NULL) {
    widget->close();
  }
}

//-----------------------------------------------------------------------------
//----------------- Public ----------------------------------------------------

//=============================================================================
// Creates QProcess object and starts the processes off then calls
//=============================================================================
bool PuttyWidget::startPuttyProcess(QString puttyPath, QString session, QString username, QString password)
{
  FUNC_DEBUG;
  try
  {
    if (!puttyPath.isEmpty() && !session.isEmpty() && !username.isEmpty() && !password.isEmpty()) {
      procPutty = new QProcess(widget);
      connect(procPutty,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(processStateChanged(QProcess::ProcessState)));
      this->setWindowTitle(session);

      QStringList args;
      args << "-load" << session;
      args << "-l" << username;
      args << "-pw" << password;
      args << "-P" << "22";

      procPutty->start(puttyPath,args);

      if (procPutty->waitForStarted(4000)) {
        Thread::msleep(msecs);
        return addProcToWidget();
      } else {
        procPutty->kill();
        return false;
      }
    } else {
      return false;
    }
  } catch (std::exception &e) {
    DEBUG << "exception " << e.what();
    return false;
  }
}

//=============================================================================
// returns if QProcess is running
//=============================================================================
bool PuttyWidget::isRunning()
{
  FUNC_DEBUG;
  return (procPutty->state() == QProcess::Running);
}

//=============================================================================
// Sets the title of the dock widget window
//=============================================================================
void PuttyWidget::setTitle(QString title)
{
  FUNC_DEBUG;
  this->setWindowTitle(title);
}

//=============================================================================
// Sets the title of the dock widget window
//=============================================================================
void PuttyWidget::setPause(unsigned long msec)
{
  FUNC_DEBUG;
  if (msec <= 0) { return; }
  if (msec > 2000) { msec = 2000; }
  this->msecs = msec;
}

//=============================================================================
// Sets the title of the dock widget window
//=============================================================================
QString PuttyWidget::getTitle()
{
  FUNC_DEBUG;
  return this->windowTitle();
}

//=============================================================================
// Kills PuTTY's process and waits for finish
//=============================================================================
bool PuttyWidget::kill()
{
  FUNC_DEBUG;
  try
  {
    if (procPutty != NULL) {
      procPutty->kill();
      return procPutty->waitForFinished(3000);
    } else {
      return true;
    }
  } catch (std::exception &e) {
    DEBUG << "exception " << e.what();
    return false;
  }
}

//=============================================================================
// Calls WINAPI::BringWindowToTop(HWND) to bring PuTTY window to the foreground
//=============================================================================
bool PuttyWidget::focus()
{
  FUNC_DEBUG;  
  return ::BringWindowToTop(puttyHandle);
}

//=============================================================================
// PuttyWidget SizeHint
//=============================================================================
QSize PuttyWidget::sizeHint() const
{
  return QSize(400, 500);
}

//-----------------------------------------------------------------------------
//----------------- Protected -------------------------------------------------

//=============================================================================
// Called when the widget resizes and this resizes the PuTTY window inside also
//=============================================================================
void PuttyWidget::resizeEvent(QResizeEvent* event)
{
  if (attached) {
    ::MoveWindow(puttyHandle,1,1,this->widget->width(),this->widget->height(),true);
    event->accept();
  }
}

//-----------------------------------------------------------------------------
//----------------- Private ---------------------------------------------------

//=============================================================================
// Uses WINAPI calls to set PuTTY process into the tab
//=============================================================================
bool PuttyWidget::addProcToWidget()
{
  FUNC_DEBUG;
  try
  {
    puttyHandle = findPuttyWindow(procPutty);
    int retries = 0;
    while( !puttyHandle && procPutty)
    {
      Thread::usleep(250000);
      puttyHandle = findPuttyWindow(procPutty);
      retries++;
      if (retries > 10) {
        return false;
      }
    }
    if ( puttyHandle != NULL ) {
      puttyParent = ::SetParent(puttyHandle,(HWND)this->widget->winId());
      if (puttyParent == NULL) {
        return false;
      }
      this->adjustSize();
      if (::ShowWindow(puttyHandle,SW_MAXIMIZE)) {
        ::SetWindowLongPtr(puttyHandle,GWL_STYLE,::GetWindowLong(puttyHandle, GWL_STYLE) & ~(WS_BORDER | WS_DLGFRAME | WS_THICKFRAME | WS_VSCROLL));
        ::SetWindowLongPtr(puttyHandle,GWL_EXSTYLE,::GetWindowLong(puttyHandle, GWL_EXSTYLE) & ~(WS_BORDER | WS_DLGFRAME | WS_THICKFRAME | WS_VSCROLL));
        attached = true;
        this->resize(this->widget->width(),this->widget->height());
        return true;
      }
    }
  } catch (std::exception &e) {
    DEBUG << "exception " << e.what();
  }

  return false;
}

//=============================================================================
// Find the PuTTY window Handle (HWND) for a given process
//=============================================================================
HWND PuttyWidget::findPuttyWindow(QProcess* process)
{
  FUNC_DEBUG;
  bool found = false;
  try
  {
    if( !process )
    {
      return 0;
    }
    if( !process->pid() )
    {
      return 0;
    }

    HWND h = ::GetTopWindow(0);
    while ( h )
    {
      DWORD pid;
      ::GetWindowThreadProcessId( h, &pid );

      if ( pid == process->pid()->dwProcessId )
      {
        char buffer[255];
        ::GetWindowTextA(h,(char*)&buffer,255);
        HWND parent = ::GetParent(h);
        WINDOWINFO info;
        ::GetWindowInfo(h,&info);

        if( parent == 0 )
        {
          found = true;
          break;
        }
      }
      h = ::GetNextWindow( h , GW_HWNDNEXT);
    }

    if( found )
      return h;
    else
      return NULL;

  } catch (std::exception &e) {
    DEBUG << "exception " << e.what();
    return NULL;
  }
}

//-----------------------------------------------------------------------------
//----------------- Slots -----------------------------------------------------

//=============================================================================
// Slot that will signal that the process' state has changed and if not running
// call SLOT to let parents know
//=============================================================================
void PuttyWidget::processStateChanged(QProcess::ProcessState state)
{
  FUNC_DEBUG;
  if (state == QProcess::NotRunning)
  {
    emit processStoppedSignal(this);
  }
}

//=============================================================================
// Slot that emits the change title triggered
//=============================================================================
void PuttyWidget::changeTitle_triggered()
{
  FUNC_DEBUG;
  emit changeTitleSignal(this);
}

//=============================================================================
// Slot for catching when the context menu is requested
//=============================================================================
void PuttyWidget::customContextMenuRequested(const QPoint &pos)
{
  FUNC_DEBUG;
  if (menuContext != NULL) {
    QPoint globalPos = this->mapToGlobal(pos);
    menuContext->exec(globalPos);
  }
}
