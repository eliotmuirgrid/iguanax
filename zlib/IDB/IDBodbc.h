#ifndef __IDB_ODBC_H__
#define __IDB_ODBC_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBodbc
//
// Description:
//
// IDBodbc class - copied IDBmysql IDBoracle.
//
// Author: Julian Muir
// Date:   Monday, April 20th, 2021
//
//---------------------------------------------------------------------------

#include <COL/COLstring.h>
#include <COL/COLarray.h>

class IDBodbc{
public:
   IDBodbc() : WarnAboutNewOdbcDlls(true) {}
   // This is non virtual because we are assuming no classes will inherit from this class
   ~IDBodbc() {}

   void setKnownOdbcDlls(const COLarray<COLstring>&);
   void addKnownOdbcDll (const COLstring&);
   const COLarray<COLstring>& knownOdbcDlls() const { return KnownOdbcDlls; }

   void setPreferredOdbcDll(const COLstring&);
   const COLstring& preferredOdbcDll() const { return PreferredOdbcDll; }

   void setWarnAboutNewOdbcDlls(bool Warn) { WarnAboutNewOdbcDlls = Warn; }
   bool warnAboutNewOdbcDlls() const { return WarnAboutNewOdbcDlls; }

private:
   COLarray<COLstring> KnownOdbcDlls;
   COLstring            PreferredOdbcDll;
   bool                 WarnAboutNewOdbcDlls;

   IDBodbc(const IDBodbc& Orig);            // not allowed
   IDBodbc& operator=(const IDBodbc& Orig); // not allowed
};

#endif // end of defensive include
