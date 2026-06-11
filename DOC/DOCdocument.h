#ifndef __DOC_MODEL_H__
#define __DOC_MODEL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCdocument
//
// Description:
//
// Concrete data model for the DOC system
//
// Author: Eliot Muir
// Date:   Tuesday 19 August 2025 - 09:01AM
// ---------------------------------------------------------------------------

#include <COL/COLstring.h>
#include <COL/COLarray.h>
#include <COL/COLhashmap.h>

class COLstring;

class DOCrelation{
public:
   DOCrelation() {};
   DOCrelation(const COLstring &iId, const COLstring &iRelation);
   COLstring Id;
   COLstring Created;
   COLstring Relation;
   // Use DOCreadRelations to populate the title
   COLstring Title;
};

bool DOCrelationEquals(const DOCrelation& Lhs, const DOCrelation& Rhs);

class DOCdocument{
public:
   DOCdocument() : Created(0) {}

   void addTag(const COLstring& Tag);

   // META DATA
   COLstring Id;
   COLstring Source;
   COLuint64 Created;
   COLstring Markup;  // This is our translation of the document content and may be wrong!
   COLstring Title;
   COLarray<DOCrelation>      Relations;
   COLhashmap<COLstring, int> Tags;

   // FOR SEARCH
   COLstring TagSearch;  // useful for search.

   // THIS IS NOT META DATA - it's the original file.
   COLstring Data;    // Data is the raw original data and while
};

COLostream& operator<<(COLostream& Stream, const DOCdocument& Doc);

COLstring DOCnewId(const DOCdocument& Doc);

void DOCdocumentCreateRandom(DOCdocument* pDocument);

// This checking functions return false if a document type, role, relation is given which isn't supported
bool DOCcheckRelation(const COLstring& Relation);

void DOCdocumentSanitizeTags(DOCdocument* pDoc);

void DOCdocumentCompileTagSearch(DOCdocument* pDoc);

void DOCdocumentMergeTags     (const DOCdocument& Source, DOCdocument* pTarget);
void DOCdocumentMergeRelations(const DOCdocument& Source, DOCdocument* pTarget);


void DOCdocumentAddress(const DOCdocument& Document, const COLstring& Component, COLvar* pResult);

void DOCdocumentAddDateTag(DOCdocument* pDoc);


#endif // end of defensive include
