#ifndef __NTAB_COLLECTION_G_H__
#define __NTAB_COLLECTION_G_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABcollectionG
//
// Description:
//
// NTABcollectionG - basically a list of tables associated with a message.
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 12:18:14 PM
//
//---------------------------------------------------------------------------

#include <COL/COLarray.h>

class NTABschema;
class NTABtableG;

class NTABcollectionG 
{
public:
   NTABcollectionG(const NTABschema& Schema, const COLstring& Name);
   // This is non virtual because we are assuming no classes will inherit from this class
   ~NTABcollectionG();

   const COLstring& name() const { return m_Name; }

   int countOfTable() const { return m_TableSet.size(); }
   
   const NTABtableG& table(int i) const;
   int tableIndex(int i) const;
   void setTableIndex(int i, int j);

   void addTable(const COLstring& Name);
private:
   NTABcollectionG(const NTABcollectionG& Orig);
   NTABcollectionG& operator=(const NTABcollectionG& Orig);

   COLstring m_Name;
   COLarray<int> m_TableSet;
   const NTABschema& m_Schema;
};

COLostream& operator<<(COLostream& Stream, const NTABcollectionG& Collection);


#endif // end of defensive include
