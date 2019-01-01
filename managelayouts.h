/*
 * managelayouts.h:
 * AutoPutty: Auto-logon/Tabbed PuTTy
 * Copyright (c) 2017-2019 Justin Whitmore/Travis Kramper
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

#ifndef MANAGELAYOUTS_H
#define MANAGELAYOUTS_H

#include <QtWidgets/QDialog>

#include <docklayout.h>

namespace Ui {
class manageLayouts;
}

class manageLayouts : public QDialog
{
  Q_OBJECT

public:
  explicit manageLayouts(QWidget *parent = 0);
  ~manageLayouts();

  void setLayouts(dockLayout* dl);


private:
  Ui::manageLayouts *ui;
  dockLayout* layouts;

  void refresh();

private slots:
  void on_listLayouts_currentTextChanged(const QString& currentText);

  void on_btnRename_clicked();
  void on_btnDelete_clicked();


};

#endif // MANAGELAYOUTS_H
