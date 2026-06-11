#ifndef __IDB_ORACLE_H__
#define __IDB_ORACLE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBoracle
//
// Description:
//
// IDBoracle class - adapted from DBCoracle
//
// Author: Vismay Shah
// Date:   Monday, April 5th, 2021 @ 02:30:42 PM
//
//---------------------------------------------------------------------------
#include <COL/COLstring.h>
#include <COL/COLarray.h>

class IDBoracle{
public:
   IDBoracle() : WarnAboutNewOracleDlls(true) {}
   // This is non virtual because we are assuming no classes will inherit from this class
   ~IDBoracle() {}

   void setKnownOracleDlls(const COLarray<COLstring>&);
   void addKnownOracleDll (const COLstring&);
   const COLarray<COLstring>& knownOracleDlls() const { return KnownOracleDlls; }
   
   void setPreferredOracleDll(const COLstring&);
   const COLstring& preferredOracleDll() const { return PreferredOracleDll; }

   void setWarnAboutNewOracleDlls(bool Warn) { WarnAboutNewOracleDlls = Warn; } 
   bool warnAboutNewOracleDlls() const { return WarnAboutNewOracleDlls; }

private:
   COLarray<COLstring> KnownOracleDlls;
   COLstring            PreferredOracleDll;
   bool                 WarnAboutNewOracleDlls;

   IDBoracle(const IDBoracle& Orig);            // not allowed
   IDBoracle& operator=(const IDBoracle& Orig); // not allowed
};

#endif // end of defensive include
