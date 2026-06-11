#ifndef __NODB_RESULT_SET_H__
#define __NODB_RESULT_SET_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODBresultSet
//
// Description:
//
// NODBresultSet class.
//
// Author: Kevin Senn
// Date:   Thursday, June 17th, 2010 @ 03:17:43 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "NODBconstant.h"

#include <NOD/NODvector.h>

#include <DB/DBresultSet.h>

class NODBresultSet : public NODvector
{
public:
   NODBresultSet(DBresultSet* pResultSet);

   // This is non virtual because we are assuming no classes will inherit from this class
   ~NODBresultSet() {}

   int protocolType() const { return NODB_DB; }
   int nodeType() const { return NODB_RESULT_SET; }
   const COLstring& protocolName() const { return NODBprotocolName; }
   const COLstring& nodeTypeName() const;

   const COLstring& name() const;

   bool isReadOnly() const { return true; }
   bool isHomogeneous() const { return true; }

protected:
   NODplace* newChild(int i, const char* Arg1, const char* Arg2);
   NODplace* initChild(int i) const;

private:
   COLref<DBresultSet> m_pResultSet;

   NODBresultSet(const NODBresultSet& Orig); // not allowed
   NODBresultSet& operator=(const NODBresultSet& Orig); // not allowed
};

void NODBregisterResultSetType();

#endif // end of defensive include
