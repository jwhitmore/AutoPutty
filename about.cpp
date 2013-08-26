/*
 * about.cpp:
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

#include <QPixmap>
#include <QLabel>

#include "version.h"
#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::About)
{
  ui->setupUi(this);
  ui->lblAutoPutty->setText(QString("AutoPutty %1.%2.%3").arg(Version::MAJOR).arg(Version::MINOR).arg(Version::PATCH));
  ui->lblRevision->setText(QString("Revision: %1").arg(Version::REVISION));
  ui->lblBuild->setText(QString("Build: %1").arg(Version::BUILD));
}

About::~About()
{
  delete ui;
}
