#ifndef __NTAB_TABLE_H__
#define __NTAB_TABLE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABtable
//
// Description:
//
// NTABtable - NODplace instance of a table - consists of a vector of rows.
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 12:15:48 PM
//
//---------------------------------------------------------------------------

#include <NOD/NODvector.h>

#include <NTAB/NTABconstant.h>

class NTABtableG;

class NTABtable : public NODvector
{
public:
   NTABtable(const NTABtableG* pGrammar);
   virtual ~NTABtable();

   virtual int protocolType() const { return NTAB_TABLE_PROTOCOL; }
   virtual int nodeType() const { return NTAB_TABLE; }
   virtual const COLstring& protocolName() const { return NTABprotocolName; }

   virtual bool isReadOnly() const { return false; }

   virtual const COLstring& nodeTypeName() const;

   virtual const COLstring& name() const;

   virtual bool isHomogeneous() const { return true; }
protected:
   virtual NODplace* newChild(int i, const char* Arg1, const char* Arg2);
   virtual NODplace* initChild(int i) const;
   
private:
   const NTABtableG* m_pGrammar;
   NTABtable(const NTABtable& Orig); // not allowed
   NTABtable& operator=(const NTABtable& Orig); // not allowed
};

void NTABregisterTableType();

#endif // end of defensive include
