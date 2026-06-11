#ifndef __NODB_VALUE_H__
#define __NODB_VALUE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODBvalue
//
// Description:
//
// NODBvalue class.
//
// Author: Kevin Senn
// Date:   Thursday, June 17th, 2010 @ 03:18:17 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "NODBconstant.h"

#include <DB/DBresultSet.h>

#include <NOD/NODsimple.h>

class DBresultSet;

class NODBvalue : public NODsimple
{
public:
   NODBvalue(DBresultSet* pResultSet, int Row, int Column);
   // This is non virtual because we are assuming no classes will inherit from this class
   ~NODBvalue() {}

   int protocolType() const { return NODB_DB; }
   int nodeType() const;
   const COLstring& protocolName() const { return NODBprotocolName; }
   const COLstring& nodeTypeName() const;

   const COLstring& name() const;

   bool isReadOnly() const { return true; }

   bool isNull() const;

   void setValue(const COLstring& Value) { COLASSERT(!isReadOnly()); }
   const COLstring& value() const { return m_CachedValue; }

private:
   COLstring m_CachedValue;
   COLref<DBresultSet> m_pResultSet;
   int m_Row;
   int m_Column;

   NODBvalue(const NODBvalue& Orig); // not allowed
   NODBvalue& operator=(const NODBvalue& Orig); // not allowed
};

void NODBvalueRegisterTypes();

#endif // end of defensive include
