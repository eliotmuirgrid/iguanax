#ifndef __NTAB_SCHEMA_H__
#define __NTAB_SCHEMA_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABschema
//
// Description:
//
// NTABschema - the root of what we'd think of as the 'vmd'.  Basically
// a list of 'messages' and the associated group of tables.
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 12:15:36 PM
//
//---------------------------------------------------------------------------

#include <COL/COLhashmap.h>
#include <COL/COLarray.h>
#include <COL/COLref.h>

#include "NTABtableG.h"
#include "NTABcollectionG.h"
class DBcollection;
class NTABschema 
{
public:
   NTABschema(DBcollection* pGrammar);
   // This is non virtual because we are assuming no classes will inherit from this class
   ~NTABschema();

   const NTABcollectionG* tableCollection(const COLstring& Name) const;
   NTABcollectionG* tableCollection(const COLstring& Name);
   NTABcollectionG& addTableCollection(const COLstring& Name);

   COLindex firstTableCollection() const;
   COLindex nextTableCollection(COLindex i) const;
   const NTABcollectionG& tableCollection(COLindex i) const;
   NTABcollectionG& tableCollection(COLindex i);   

   int countOfCollection() const { return m_CollectionTable.size(); }

   // Not bothering with remove methods since we only load from a vmd file.

   int countOfTable() const;
   const NTABtableG& table(int i) const;
   NTABtableG& table(int i);
   NTABtableG& addTable();

   int tableIndex(const COLstring& Name) const;

   void clear();

private:
   COLhashmap<COLstring, NTABcollectionG* > m_CollectionTable;
   COLarray< COLref<NTABtableG> > m_TableVector;

   NTABschema(const NTABschema& Orig); // not allowed
   NTABschema& operator=(const NTABschema& Orig); // not allowed
};

COLostream& operator<<(COLostream& Stream, const NTABschema& Schema);


#endif // end of defensive include
