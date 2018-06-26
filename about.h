/*
 * about.h:
 * AutoPutty: Auto-logon/Tabbed PuTTy
 * Copyright (c) 2012-2018 Justin Whitmore
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

#ifndef ABOUT_H
#define ABOUT_H

#include <QtWidgets/QDialog>

namespace Ui {
class About;
}

class About : public QDialog
{
  Q_OBJECT
  
public:
  explicit About(QWidget *parent = 0);
  ~About();
  
private slots:
  void on_lblEmail_linkActivated(const QString &link);
  void on_lblDownloads_linkActivated(const QString &link);
  void on_btnSupport_clicked();

private:
  Ui::About *ui;
};

#endif // ABOUT_H
