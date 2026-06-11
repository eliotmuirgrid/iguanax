#ifndef __EDI_GRAMMAR_H__
#define __EDI_GRAMMAR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// EDI grammar for X12/EDI/HL7
//
// Author:   Eliot Muir
// Date:     26th July 2022
//---------------------------------------------------------------------------

#include <COL/COLstring.h>
#include <COL/COLarray.h>
#include <COL/COLmap.h>

enum EDItype{
   EDI_MESSAGE=0,
   EDI_SEGMENT_GROUP=1,
   EDI_SEGMENT=2, 
   EDI_COMPOSITE=3
};

const char* EDItypeAsString(EDItype Type);
EDItype EDIstringToType(const COLstring& Type);

class EDInode;

class EDIchild{
public:
   EDIchild(EDInode* pNode, const COLstring& Description, bool Required=false, bool Repeating=false);
   bool isRepeating() const; // TODO - just return the member directly
   EDInode* pNode;
   COLstring Description;
   bool      Required=false;
   bool      Repeats=false;
};

// Helper function to assess if we have infinite repeats allowed.
// inline bool EDIunlimitedRepeats(const EDIchild& Child) { return Child.Maximum == -1; }

class EDInode{
public:
   EDInode(const COLstring& Name, const COLstring& Description, EDItype Type);
   EDInode();
   COLstring           Name;
   COLstring           Description;
   EDItype             Type;
   COLarray<EDIchild> Children;
};

COLostream& operator<<(COLostream& Stream, const EDInode& Node);

class EDImatch{
public:
   COLstring Match;         // Glob expression to match MSH.9 against.
   COLstring MessageName;   // Name of message to match against.
};

class EDIcollection{
public:
   EDIcollection();
   ~EDIcollection();
   void clear();
   COLmap<COLstring, EDInode*> Nodes;   // owned by this object.

   COLarray<EDImatch> MatchRules;  // maps MSH.9 glob expression to a message name.
};

// Defined in EDIsave since this is where the code lies.
COLostream& operator<<(COLostream& Stream, const EDIcollection& Col);

#endif // end of defensive include
