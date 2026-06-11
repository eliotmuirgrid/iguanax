#ifndef __DBD_INIT_H__
#define __DBD_INIT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDinit
//
// Description:
//
// This handles starting of the DBDservice and related helper functions.
//
// Author: Tyler Brown
// Date:   Feb 25nd, 2021
//---------------------------------------------------------------------------
#include <COL/COLstring.h>

class DBDworld;
class COLvar;

// LogEncryptionKey is required if log encryption is on
void DBDinitNormal(DBDworld* pWorld, const COLstring& LogEncryptionKey = "");
void DBDinitNormalWithEncryption(DBDworld* pService);
void DBDinitInstall(DBDworld* pService, bool Quiet);

#endif // end of defensive include
