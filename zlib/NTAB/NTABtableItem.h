#ifndef __NTAB_TABLE_ITEM_H__
#define __NTAB_TABLE_ITEM_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABtableItem
//
// Description:
//
// NTABtableItem - leaf value.
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 12:27:09 PM
//
//---------------------------------------------------------------------------

#include <NOD/NODsimple.h>
#include "NTABconstant.h"

#include <COL/COLdateTime.h>

class NTABtableG;

class NTABtableItem : public NODsimple{
public:
   NTABtableItem(NTABtableG* pGrammar, int Column);
   virtual ~NTABtableItem();

   int protocolType() const { return NTAB_TABLE_PROTOCOL; }
   virtual int nodeType() const;
   const COLstring& protocolName() const { return NTABprotocolName; }

   bool isReadOnly() const { return false; }

   bool isNull() const { return m_IsNull; }
   void setIsNull(bool IsNull);

   bool isKey() const;

   virtual const COLstring& nodeTypeName() const;
   virtual const COLstring& name() const;
 
   const COLstring& value() const;

protected:
   COLstring m_Value;
private:
   COLref<NTABtableG> m_pGrammar;
   int m_Column;
   bool m_IsNull;
};

class NTABdateTimeItem : public NTABtableItem{
public:
   NTABdateTimeItem(NTABtableG* pGrammar, int Column);
   virtual void setValue(const COLstring& Value);
};

class NTABdoubleItem : public NTABtableItem{
public:
   NTABdoubleItem(NTABtableG* pGrammar, int Column);
   virtual void setValue(const COLstring& Value);
};

class NTABintItem : public NTABtableItem{
public:
   NTABintItem(NTABtableG* pGrammar, int Column);
   virtual void setValue(const COLstring& Value);
};

class NTABstringItem : public NTABtableItem{
public:
   NTABstringItem(NTABtableG* pGrammar, int Column);
   virtual void setValue(const COLstring& Value);
};

NODplace* NTABnewTableItem(NTABtableG* pGrammar, int Column);

#endif // end of defensive include
