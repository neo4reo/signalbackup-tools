/*
    Copyright (C) 2019  Selwin van Dijk

    This file is part of signalbackup-tools.

    signalbackup-tools is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    signalbackup-tools is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with signalbackup-tools.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "fileencryptor.ih"

FileEncryptor::FileEncryptor(std::string const &passphrase, unsigned char *salt, uint64_t salt_size, unsigned char *iv, uint64_t iv_size)
  :
  d_passphrase(passphrase)
{
  d_ok = init(salt, salt_size, iv, iv_size);
}

FileEncryptor::FileEncryptor(std::string const &passphrase)
  :
  d_passphrase(passphrase)
{}

FileEncryptor::FileEncryptor()
{}

bool FileEncryptor::init(std::string const &passphrase, unsigned char *salt, uint64_t salt_size, unsigned char *iv, uint64_t iv_size)
{
  d_passphrase = passphrase;
  return init(salt, salt_size, iv, iv_size);
}

bool FileEncryptor::init(unsigned char *salt, uint64_t salt_size, unsigned char *iv, uint64_t iv_size)
{
  // set salt;
  d_salt_size = salt_size;
  d_salt = new unsigned char[d_salt_size];
  std::memcpy(d_salt, salt, d_salt_size);

  // set iv;
  d_iv_size = iv_size;
  d_iv = new unsigned char[d_iv_size];
  std::memcpy(d_iv, iv, d_iv_size);

  d_counter = fourBytesToUint(d_iv);

  // std::cout << "IV  : " << bepaald::bytesToHexString(d_iv, d_iv_size) << std::endl;
  // std::cout << "SALT: " << bepaald::bytesToHexString(d_salt, d_salt_size) << std::endl;

  // generate backup key from salt and passphrase
  if (!getBackupKey(d_passphrase))
    return false;

  // generate mackey and cipher from backupkey
  if (!getCipherAndMac(32, 64))
    return false;

  DEBUGOUT("IV: ", bepaald::bytesToHexString(d_iv, d_iv_size));
  DEBUGOUT("SALT: ", bepaald::bytesToHexString(d_salt, d_salt_size));
  DEBUGOUT("BACKUPKEY: ", bepaald::bytesToHexString(d_backupkey, d_backupkey_size));
  DEBUGOUT("CIPHERKEY: ", bepaald::bytesToHexString(d_cipherkey, d_cipherkey_size));
  DEBUGOUT("MACKEY: ", bepaald::bytesToHexString(d_mackey, d_mackey_size));
  DEBUGOUT("COUNTER: ", d_counter);

  d_ok = true;
  return d_ok;
}