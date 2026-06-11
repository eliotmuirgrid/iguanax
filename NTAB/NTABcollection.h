#ifndef __NTAB_COLLECTION_H__
#define __NTAB_COLLECTION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABcollection
//
// Description:
//
// NTABcollection - root node - children are list of tables..
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 12:15:41 PM
//
//---------------------------------------------------------------------------

#include <NOD/NODcomplex.h>

#include <NTAB/NTABconstant.h> 

class NTABcollectionG;
class NTABschema;

class NTABcollection : public NODcomplex{
public:
   NTABcollection(const NTABcollectionG* pGrammar);
   virtual ~NTABcollection();

   int protocolType() const { return NTAB_TABLE_PROTOCOL; }
   virtual int nodeType() const { return NTAB_ROOT; }
   const COLstring& protocolName() const { return NTABprotocolName; }

   bool isReadOnly() const { return false; }
   bool favorIndexByName() const { return true; }

   bool isNull() const { return false; } // never null

   virtual const COLstring& nodeTypeName() const;

   virtual const COLstring& name() const;

   COLstring treeValue() const;

protected:
   NODplace* newChild(int i, const char* Arg1, const char* Arg2) { COL_ERROR_STREAM_PLAIN("New children cannot be added to this node.", COLerror::PreCondition); }
   NODplace* initChild(int i) const;
   virtual bool onRemoveChild(int i) { return false; } // tells NODplaceComposite that rather than 'removing' the child, we should just 'nullify' it.

private:
   const NTABcollectionG* m_pGrammar;

   NTABcollection(const NTABcollection& Orig); // not allowed
   NTABcollection& operator=(const NTABcollection& Orig); // not allowed
};

// Will throw if the message doesn't exist.
const NTABcollectionG* NTABcollectionByName(const NTABschema& Schema, const COLstring& Name);

void NTABregisterCollectionType();

#endif // end of defensive include
