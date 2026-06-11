#ifndef __DBE_ENCRYPT_STRING_H__
#define __DBE_ENCRYPT_STRING_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBEencryptString
//
// Description:
//
// DBEencryptString class.
//
// Author: Nasron Cheong
// Date:   Wednesday, April 23rd, 2008 @ 06:26:27 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLstring;

class DBEencryptionKey
{
public:
   DBEencryptionKey();
   ~DBEencryptionKey();
   //The key is a 16 byte value
   COLuint8 Value[16];
};


// generates a random 24 byte long salt, which is really a 16 byte random number encoded 
// in base 64.
//
// this is usually stored in the config file
//
COLstring DBEgenerateEncryptionSalt();

const COLstring& DBEencryptionKeySeed();

//returns the encryption key used for encrypt/decrypt
DBEencryptionKey DBEcreateEncryptionKey( const COLstring& Salt );

void DBEencryptString(const COLstring& Input, const DBEencryptionKey& EncryptionKey, COLstring* pOutput);
void DBEencryptString(const COLstring& Input, COLstring* pOutput);
COLstring DBEencryptString(const COLstring& Input, const DBEencryptionKey& EncryptionKey);

//if success, true is returned and output contains the decrypted string
//if failed, false is returned, and output contains the reason for failure
bool DBEdecryptString(const COLstring& InputData, const DBEencryptionKey& Key, COLstring* pOutput);
bool DBEdecryptString(const COLstring& InputData, COLstring* pOutput);

#endif // end of defensive include

