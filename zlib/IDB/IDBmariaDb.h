#ifndef __IDB_MARIA_DB_H__
#define __IDB_MARIA_DB_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBmariaDb
//
// Description:
//
// IDBmariaDb class - copied IDBmysql IDBoracle.
//
// Author: Julian Muir
// Date:   Monday, April 26th, 2021
//
//---------------------------------------------------------------------------

#include <COL/COLstring.h>
#include <COL/COLarray.h>

class IDBmariaDb{
public:
   IDBmariaDb() : WarnAboutNewMariaDbDlls(true) {}
   // This is non virtual because we are assuming no classes will inherit from this class
   ~IDBmariaDb() {}

   void setKnownMariaDbDlls(const COLarray<COLstring>&);
   void addKnownMariaDbDll (const COLstring&);
   const COLarray<COLstring>& knownMariaDbDlls() const { return KnownMariaDbDlls; }

   void setPreferredMariaDbDll(const COLstring&);
   const COLstring& preferredMariaDbDll() const { return PreferredMariaDbDll; }

   void setWarnAboutNewMariaDbDlls(bool Warn) { WarnAboutNewMariaDbDlls = Warn; }
   bool warnAboutNewMariaDbDlls() const { return WarnAboutNewMariaDbDlls; }

private:
   COLarray<COLstring> KnownMariaDbDlls;
   COLstring            PreferredMariaDbDll;
   bool                 WarnAboutNewMariaDbDlls;

   IDBmariaDb(const IDBmariaDb& Orig);            // not allowed
   IDBmariaDb& operator=(const IDBmariaDb& Orig); // not allowed
};

#endif // end of defensive include
