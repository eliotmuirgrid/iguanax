#ifndef __IDB_MY_SQL_H__
#define __IDB_MY_SQL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBmySql
//
// Description:
//
// IDBmySql class - adapted from DBCmySql.
//
// Author: Vismay Shah
// Date:   Monday, April 5th, 2021 @ 02:30:42 PM
//
//---------------------------------------------------------------------------

#include <COL/COLstring.h>
#include <COL/COLarray.h>

class IDBmySql{
public:
   IDBmySql() : WarnAboutNewMySqlDlls(true) {}
   // This is non virtual because we are assuming no classes will inherit from this class
   ~IDBmySql() {}

   void setKnownMySqlDlls(const COLarray<COLstring>&);
   void addKnownMySqlDll (const COLstring&);
   const COLarray<COLstring>& knownMySqlDlls() const { return KnownMySqlDlls; }

   void setPreferredMySqlDll(const COLstring&);
   const COLstring& preferredMySqlDll() const { return PreferredMySqlDll; }

   void setWarnAboutNewMySqlDlls(bool Warn) { WarnAboutNewMySqlDlls = Warn; }
   bool warnAboutNewMySqlDlls() const { return WarnAboutNewMySqlDlls; }

private:
   COLarray<COLstring> KnownMySqlDlls;
   COLstring            PreferredMySqlDll;
   bool                 WarnAboutNewMySqlDlls;

   IDBmySql(const IDBmySql& Orig);            // not allowed
   IDBmySql& operator=(const IDBmySql& Orig); // not allowed
};

#endif // end of defensive include
