/*
 * crypt.cpp:
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

#include <QByteArray>

#include "debug.h"
#include "crypt.h"

//=============================================================================
//=============================================================================
Crypt::Crypt()
{
  FUNC_DEBUG;
  _ikey = 0;
  _key.clear();
}

//=============================================================================
//=============================================================================
Crypt::Crypt(QString passphrase)
{
  FUNC_DEBUG;
  _ikey = 0;
  _key.clear();
  genKey(passphrase);
}

//=============================================================================
//=============================================================================
Crypt::~Crypt()
{
  FUNC_DEBUG;
}

//=============================================================================
//=============================================================================
void Crypt::genKey(QString passphrase)
{
  FUNC_DEBUG;
  _ikey = passphrase.size();
  for (int i = 0; i < passphrase.size(); i++) {
    _ikey += (int)passphrase.at(i).toLatin1();
  }
  _key.resize(8);
  for (int i = 0; i < _key.size(); i++) {
    int byte = (_ikey >> i) ^ (int)passphrase.at(i % (passphrase.size() - 1)).toLatin1();
    byte = byte & 0xFF;
    _key[i] = static_cast<char>(byte);
  }
}

//=============================================================================
//=============================================================================
QString Crypt::encrypt(QString str)
{
  FUNC_DEBUG;  
  if (_key.isEmpty() || str.isEmpty()) { return QString(); }
  
  QByteArray baEncrypt = str.toLatin1();
  for (int i = 0; i < baEncrypt.count(); i++) {
    baEncrypt[i] = baEncrypt.at(i) ^ _key.at(i % _key.size());
  }
  baEncrypt = qCompress(baEncrypt,9);
  return QString(baEncrypt.toBase64());
}

//=============================================================================
//=============================================================================
QString Crypt::decrypt(QString str)
{
  FUNC_DEBUG;  
  if (_key.isEmpty() || str.isEmpty()) { return QString(); }
  
  QByteArray baDecrypt = QByteArray::fromBase64(str.toLatin1());
  baDecrypt = qUncompress(baDecrypt);
  for (int i = 0; i < baDecrypt.count(); i++) {
    baDecrypt[i] = baDecrypt.at(i) ^ _key.at(i % _key.size());
  }
  return QString(baDecrypt);
}

//=============================================================================
//=============================================================================
QString Crypt::encrypt(QString passphrase, QString str)
{
  FUNC_DEBUG;
  Crypt crypt(passphrase);
  return crypt.encrypt(str);
}

//=============================================================================
//=============================================================================
QString Crypt::decrypt(QString passphrase, QString str)
{
  FUNC_DEBUG;
  Crypt crypt(passphrase);
  return crypt.decrypt(str);
}
