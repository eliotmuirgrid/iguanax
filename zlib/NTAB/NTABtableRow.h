#ifndef __NTAB_TABLE_ROW_H__
#define __NTAB_TABLE_ROW_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABtableRow
//
// Description:
//
// NTABtableRow - a NODplace row!
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 12:15:51 PM
//
//---------------------------------------------------------------------------

#include "NTABconstant.h"
#include "NTABtableG.h"

#include <NOD/NODcomplex.h>

#include <COL/COLref.h>

class NTABtableG;

class NTABtableRow : public NODcomplex{
public:
   NTABtableRow(const NTABtableG* pGrammar);
   virtual ~NTABtableRow() {}
 
   int protocolType() const { return NTAB_TABLE_PROTOCOL; }
   virtual int nodeType() const { return NTAB_TABLE_ROW; }
   const COLstring& protocolName() const { return NTABprotocolName; }

   bool isReadOnly() const { return false; }
   bool favorIndexByName() const { return true; }

   virtual const COLstring& nodeTypeName() const;

   virtual const COLstring& name() const;

   bool isNull() const { return m_IsNull; }
   void setIsNull(bool IsNull) { m_IsNull = IsNull; }

   virtual int childIndex(const COLstring& Name) const;

protected:
   // TODO - we could support adding columns.  The two string args could represent the data type and name.
   virtual NODplace* newChild(int i, const char* Arg1, const char* Arg2) { COL_ERROR_STREAM_PLAIN("New children cannot be added to this node.", COLerror::PreCondition); }
   virtual NODplace* initChild(int i) const;
   virtual bool onRemoveChild(int i);

private:
   COLref<NTABtableG> m_pGrammar;
   bool m_IsNull;
};

void NTABregisterRowType();

#endif // end of defensive include
