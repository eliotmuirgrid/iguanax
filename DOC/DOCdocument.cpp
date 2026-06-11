// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCdocument
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 19 August 2025 - 09:01AM
// ---------------------------------------------------------------------------

#include <DOC/DOCdocument.h>
#include <DOC/DOCserialize.h>
#include <DOC/DOChex.h>

#include <GUID/GUIDid.h>

#include <SFI/SFImd5.h>
#include <COL/COLmath.h>

#include <time.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

DOCrelation::DOCrelation(const COLstring& iId, const COLstring& iRelation)
: Id(iId), Relation(iRelation){
   COL_METHOD(DOCrelation::DOCrelation);
}

bool DOCrelationEquals(const DOCrelation& Lhs, const DOCrelation& Rhs){
   COL_FUNCTION(DOCrelationEquals);
   // we only identify on IDs which are unique.
   return Lhs.Id == Rhs.Id; 
}

COLostream& operator<<(COLostream& Stream, const DOCdocument& Doc){
   COLstring Out;
   DOCtoJson(Doc, &Out);
   Stream << Out;
   return Stream;
}

COLstring DOCnewId(const DOCdocument& Document){
   COL_FUNCTION(DOCnewId);
   //md5 of the actual data on the doc + creation date listed on the doc
   return SFImd5(Document.Data + COLuint64ToString(Document.Created)).toLowerCase();
}


COLstring DOCsanitizeTag(const COLstring& In){
   COL_FUNCTION(DOCsanitizeTag);
   COLstring Result;
   for (int i=0; i < In.size(); i++){
      char c = In[i];
      if ( (c >= 'A' && c <= 'Z') ||    // uppercase
           (c >= 'a' && c <= 'z') ||    // lowercase
           (c >= '0' && c <= '9') ||    // digits
            c == '-' || c == ':'  || c == '_'  || c== '@' || c == '.'){     // allowed punctuation
         Result += c;
      }
   }
   return Result;
}

void DOCdocumentSanitizeTags(DOCdocument* pDoc){
   COL_FUNCTION(DOCdocumentSanitizeTags);
   COLhashmap<COLstring, COLstring> Map;
   for (auto i=pDoc->Tags.begin(); i != pDoc->Tags.end(); i++){
      COLstring CleanTag = DOCsanitizeTag(i->first);
      Map.add(i->first, CleanTag);
   }
   for (auto i=Map.cbegin(); i != Map.cend(); i++){
      pDoc->Tags.removeKey(i->first);
      pDoc->addTag(i->second);
      COL_TRC("Mapping " << i->first << " -> " << i->second);
   }
   pDoc->Tags.removeKey(""); // remove blank tag.
}

void DOCdocumentCompileTagSearch(DOCdocument* pDoc){
   COL_FUNCTION(DOCdocumentCompileTagSearch);
   pDoc->TagSearch.clear();
   for (auto i=pDoc->Tags.cbegin(); i != pDoc->Tags.cend(); i++){
      pDoc->TagSearch += COL_T(" #") + i->first;
   }
}

void DOCdocumentMergeTags(const DOCdocument& Source, DOCdocument* pTarget){
   COL_FUNCTION(DOCdocumentMergeTags);
   for (auto i = Source.Tags.cbegin(); i != Source.Tags.cend(); i++){
      pTarget->addTag(i->first);
   }
}

void DOCdocumentMergeRelations(const DOCdocument& Source, DOCdocument* pTarget){
   COL_FUNCTION(DOCdocumentMergeRelations);
   DOCdocument& Target = *pTarget;
   for (int i=0; i < Source.Relations.size(); i++){
      bool NeedsRelation = true;
      for (int j=0; j < Target.Relations.size(); j++){
         if (DOCrelationEquals(Source.Relations[i], Target.Relations[j])){
            COL_TRC("Already had relationship");
            NeedsRelation = false;
         }
      }
      if (NeedsRelation){
         Target.Relations.push_back(Source.Relations[i]);
      }
   }
}

void DOCdocument::addTag(const COLstring& Tag){
   COL_METHOD(DOCdocument::addTag);
   COLstring Case(Tag);
   Case = Case.toLowerCase();
   COL_VAR(Case);
   Tags.add(Case, 0);
}


// TODO: Make doc.set merge tags by default.
// Add code to C++ which can generate the #YYYY_01MMM_DD hashtag.
// If the created date is zero then we tag as "import_error"

// Generate a git ignore which goes into the root of the destination repository 
// Commit a milestore before each operation
// Write code to compare.
