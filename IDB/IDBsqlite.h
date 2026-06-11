#ifndef __IDB_SQLITE_H__
#define __IDB_SQLITE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBsqlite
//
// Description:
//
// IDBsqlite class - copied IDBmysql IDBoracle.
//
// Author: Julian Muir
// Date:   Monday, April 26th, 2021
//
//---------------------------------------------------------------------------

#include <COL/COLstring.h>
#include <COL/COLarray.h>

class IDBsqlite{
public:
   IDBsqlite() : WarnAboutNewSqliteDlls(true) {}
   // This is non virtual because we are assuming no classes will inherit from this class
   ~IDBsqlite() {}

   void setKnownSqliteDlls(const COLarray<COLstring>&);
   void addKnownSqliteDll (const COLstring&);
   const COLarray<COLstring>& knownSqliteDlls() const { return KnownSqliteDlls; }

   void setPreferredSqliteDll(const COLstring&);
   const COLstring& preferredSqliteDll() const { return PreferredSqliteDll; }

   void setWarnAboutNewSqliteDlls(bool Warn) { WarnAboutNewSqliteDlls = Warn; }
   bool warnAboutNewSqliteDlls() const { return WarnAboutNewSqliteDlls; }

private:
   COLarray<COLstring> KnownSqliteDlls;
   COLstring            PreferredSqliteDll;
   bool                 WarnAboutNewSqliteDlls;

   IDBsqlite(const IDBsqlite& Orig);            // not allowed
   IDBsqlite& operator=(const IDBsqlite& Orig); // not allowed
};

#endif // end of defensive include
