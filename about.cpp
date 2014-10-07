/*
 * about.cpp:
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

#include <QPixmap>
#include <QLabel>
#include <QDesktopServices>
#include <QUrl>

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
  ui->lblEmail->setText("<a href='mailto:support@justinwhitmore.com'>Support Email</a>");
  ui->lblEmail->setOpenExternalLinks(true);
  ui->lblDownloads->setText("<a href='https://github.com/jwhitmore/AutoPutty/releases'>Release Downloads</a>");
  ui->lblDownloads->setOpenExternalLinks(true);
  ui->btnSupport->setMinimumSize(102,24);
  ui->btnSupport->setMaximumSize(102,24);
  ui->btnSupport->setText("");
  ui->btnSupport->setStyleSheet("background-image:url(:/images/PaypalSupportButton.png)");
  ui->btnSupport->setFlat(true);
  ui->btnSupport->setDefault(false);
  ui->btnSupport->setAutoDefault(false);
  ui->btnSupport->setFocusPolicy(Qt::NoFocus);
}

About::~About()
{
  delete ui;
}

void About::on_lblEmail_linkActivated(const QString &link)
{
  QDesktopServices::openUrl(QUrl(link));
}

void About::on_lblDownloads_linkActivated(const QString &link)
{
  QDesktopServices::openUrl(QUrl(link));
}

void About::on_btnSupport_clicked()
{
  QString url = "";

  QString business = "justin@justinwhitmore.com";
  QString description = "Donation";
  QString country = "US";
  QString currency = "USD";

  url = QString("https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=%1&lc=%2&item_name=%3&currency_code=%4&bn=PP%2dDonationsBF")
      .arg(business)
      .arg(country)
      .arg(description)
      .arg(currency);

  QDesktopServices::openUrl(QUrl(url));
}
