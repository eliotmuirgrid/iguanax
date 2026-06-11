#ifndef __NTAB_TABLE_G_H__
#define __NTAB_TABLE_G_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABtableG
//
// Description:
//
// NTABtableG - represents a table.
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 12:18:19 PM
//
//---------------------------------------------------------------------------

#include <COL/COLarray.h>
#include <COL/COLhashmap.h>
#include "NTABconstant.h"

class NTABcolumn{
public:
   NTABcolumn(const COLstring& Name, const COLstring& Description, NTABtype Type, bool IsKey=false) : m_Name(Name), m_Description(Description), m_Type(Type), m_IsKey(IsKey) {}
   const COLstring& name() const { return m_Name; } 
   void setName(const COLstring& NewValue);

   const COLstring& description() const { return m_Description; } 
   void setDescription(const COLstring& NewValue);


   NTABtype type() const { return m_Type; }
   const COLstring& typeName() const;

   bool isKey() { return m_IsKey; }
   bool isKey() const { return m_IsKey; }

private:
   COLstring m_Name, m_Description;
   bool m_IsKey;
   NTABtype m_Type;
};

const COLstring& NTABtypeName(NTABtype Type);
void NTABtableGRegisterTypes();

class NTABtableG{
public:
   NTABtableG();
   NTABtableG(const NTABtableG& Orig);
   NTABtableG& operator=(const NTABtableG& Orig);
   // This is non virtual because we are assuming no classes will inherit from this class
   ~NTABtableG();

   void AddRef() { m_CountOfRef++; }
   void Release();
   int countOfRef() { return m_CountOfRef; }

   const COLstring& name() const { return m_Name; }
   void setName(const COLstring& Name);

   const COLstring& rowName() const { return m_RowName; };  // helpful to cache this.

   int countOfColumn() const;
   const NTABcolumn& column(int i) const;
   NTABcolumn& column(int i);
   void addColumn(const COLstring& Name, const COLstring& Description, NTABtype Type, bool IsKey=false);
   void removeColumn(int i);

   void index();

   int columnIndex(const COLstring& Name) const;

private:
   COLstring m_Name, m_RowName;
   COLarray<NTABcolumn> m_ColumnVector;

   COLhashmap<COLstring, int> m_ColumnIndex;
   int m_CountOfRef;
};

COLostream& operator<<(COLostream& Stream, const NTABtableG& Table);

#endif // end of defensive include
