/*
 * main.cpp:
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

#include <QApplication>
#include <QSplashScreen>
#include "mainwindow.h"
#include "version.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  a.setApplicationVersion(APP_VERSION);
  QPixmap pixmap(":/images/AutoPutty_splash.png");
  QSplashScreen splash(pixmap);
  splash.show();
  Thread::msleep(500);
  a.processEvents();
  MainWindow w;
  splash.hide();
  w.show();

  return a.exec();
}
