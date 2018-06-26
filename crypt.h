/*
 * crypt.h:
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

#ifndef CRYPT_H
#define CRYPT_H

#include <QString>
#include <QVector>

class Crypt
{
public:
  Crypt();
  ~Crypt();
  Crypt(QString passphrase);
  void genKey(QString passphrase);
  QString encrypt(QString str);
  QString decrypt(QString str);
  static QString encrypt(QString passphrase, QString str);
  static QString decrypt(QString passphrase, QString str);

private:
  int _ikey;
  QVector<char> _key;
};

#endif // CRYPT_H
