#ifndef __NODB_ROW_H__
#define __NODB_ROW_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODBrow
//
// Description:
//
// NODBrow class.
//
// Author: Kevin Senn
// Date:   Thursday, June 17th, 2010 @ 03:18:13 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "NODBconstant.h"

#include <NOD/NODcomplex.h>

#include <DB/DBresultSet.h>

class NODBrow : public NODcomplex
{
public:
   // Row may be -1 if this is a "dummy" item.
   NODBrow(DBresultSet* pResultSet, int Row);

   // This is non virtual because we are assuming no classes will inherit from this class
   ~NODBrow() {}

   int protocolType() const { return NODB_DB; }
   int nodeType() const { return NODB_ROW; }
   const COLstring& protocolName() const { return NODBprotocolName; }
   const COLstring& nodeTypeName() const;

   const COLstring& name() const;

   bool isNull() const { return m_Row == -1; }

   bool isReadOnly() const { return true; }
   bool favorIndexByName() const { return true; }

   // We don't need to override this, but doing so will result in a faster implementation.
   int childIndex(const COLstring& Name) const;

protected:
   // Only applies to writable trees.
   NODplace* newChild(int i, const char* Arg1, const char* Arg2) { COLASSERT(!isReadOnly()); return NULL; }

   NODplace* initChild(int i) const;

private:
   COLref<DBresultSet> m_pResultSet;
   int m_Row;

   NODBrow(const NODBrow& Orig); // not allowed
   NODBrow& operator=(const NODBrow& Orig); // not allowed
};

void NODBregisterRowType();

#endif // end of defensive include
