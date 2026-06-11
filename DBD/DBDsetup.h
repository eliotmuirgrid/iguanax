#ifndef __DBD_SETUP_H__
#define __DBD_SETUP_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDsetup
//
// Description:
//
// Routines to create and destroy the C++ objects in IguanaX.
//
// Author: Eliot Muir
// Date:   Monday 19 September 2022 - 02:04PM
// ---------------------------------------------------------------------------
class DBDworld;
class SCKloop;
class COLstring;
struct CFGmap;

void DBDcreateCore(DBDworld* pWorld, const COLstring& LogEncryptionKey = "");
void DBDcreate(DBDworld* pWorld, const COLstring& LogEncryptionKey);
void DBDdestroy(DBDworld* pWorld);

#endif // end of defensive include
