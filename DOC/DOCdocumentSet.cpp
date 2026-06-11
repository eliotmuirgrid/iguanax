// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCdocumentSet
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 17 September 2025 - 01:15PM
// ---------------------------------------------------------------------------

#include <DOC/DOCdocumentSet.h>
#include <DOC/DOCdir.h>
#include <DOC/DOCdocument.h>
#include <DOC/DOChex.h>
#include <DOC/DOCserialize.h>

#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void DOCdocumentAddress(const DOCdocument& Doc, const COLstring& Component, COLvar* pResult){
   COL_FUNCTION(DOCdocumentAddress);
   COLvar& Result = *pResult;
   Result["id"] = Doc.Id;
   Result["created"] = DOCtoHex(Doc.Created);
   Result["component"] = Component;
}

static COLstring DOCdocumentDateTag(time_t UnixStamp){
   COL_FUNCTION(DOCdocumentDateTag);
   COL_VAR(UnixStamp);
   COLASSERT(UnixStamp >= 0);
   tm local;
   #if defined(_WIN32)
      localtime_s(&local, &UnixStamp);   // Windows
   #else
      localtime_r(&UnixStamp, &local);   // POSIX
   #endif

   static const char* months[] = {
      "Jan","Feb","Mar","Apr","May","Jun",
      "Jul","Aug","Sep","Oct","Nov","Dec"
   };

   COLstring year  = COLintToString(local.tm_year + 1900);
   int monthInt = local.tm_mon + 1;   // 1..12
   int dayInt   = local.tm_mday;      // 1..31
   COLstring month = COLintToString(monthInt);
   COLstring day = COLintToString(dayInt);
   
   COLstring Result;
   Result += year + "_";
   if (monthInt < 10) Result += "0";
   Result += month + "-" + months[local.tm_mon] + "_";
   if (dayInt < 10) Result += "0";
   Result += day;

   COL_VAR(Result);
   return Result;
}

void DOCdocumentAddDateTag(DOCdocument* pDoc){
   COL_FUNCTION(DOCdocumentAddDateTag);
   COLstring Tag = DOCdocumentDateTag(pDoc->Created);
   COL_VAR2(pDoc->Created, Tag);
   pDoc->addTag(Tag);
}

// needs more error checking.
void DOCdocumentSet(const COLvar& Data, COLvar* pResult){
   COL_FUNCTION(DOCdocumentSet);
   COLstring Id         = COLvarRequiredString(Data, "id");
   COLstring CreatedHex = COLvarRequiredString(Data, "created");
   COLstring Component  = COLvarRequiredString(Data, "component");
   bool      Overwrite  = COLvarGetBool       (Data, "overwrite", false);
   
   COL_VAR3(Component, Id, CreatedHex);
   DOCdocument Doc;
   DOCreadDoc(Component, Id, CreatedHex, &Doc);
   if (Data.exists("markdown")){      
      COLstring FileName = DOCfileMd(Component, Doc);
      COL_VAR2(FileName, Data["markdown"]);
      FILwriteFile(FileName, Data["markdown"]);
   }
   bool MetaUpdate=false;
   // Changing the source will change the name the source is stored under
   //if (Data.exists("source")){ Doc.Source = Data["source"]; MetaUpdate=true;}
   if (Data.exists("title")) { Doc.Title  = Data["title"];  MetaUpdate=true;}

   if (Data.exists("tags")){
      if (Overwrite){
         Doc.Tags.clear();
         COL_TRC("Replacing tags." << Data["tags"].json());
         DOCtagsFromJson(Data["tags"], &Doc);
      } else {
         COL_TRC("Merging tags" << Data["tags"]);
         DOCdocument New;
         DOCtagsFromJson(Data["tags"], &New);
         DOCdocumentMergeTags(New, &Doc);
      }
      DOCdocumentSanitizeTags(&Doc);
      COL_TRC(Doc);
      DOCdocumentAddDateTag(&Doc);
      COL_TRC(Doc);
      MetaUpdate=true;
   }
   if (Data.exists("relations")){
      if (Overwrite){
         COL_TRC("Over-writing relations");
         Doc.Relations.clear();
         COL_TRC("Replacing relations." << Data["relations"].json());
         DOCrelatedFromJson(Data["relations"], &Doc.Relations);
      } else {
         COL_TRC("Merging relations");
         DOCdocument New;
         DOCrelatedFromJson(Data["relations"], &New.Relations);
         DOCdocumentMergeRelations(New, &Doc);
      }
      MetaUpdate = true;
   }
   COL_TRC(Doc);
   if (MetaUpdate){
      COL_TRC("Saving the document");
      DOCsaveDoc(Doc,Component);
   }
   DOCdocumentAddress(Doc, Component, pResult); // Give the co-ordinates of the added document
}

void DOCwebDocumentSet(const COLwebRequest& Request){
   COL_FUNCTION(DOCwebDocumentSet);
   COL_VAR(Request.Data.json(1));
   COLvar Result;
   DOCdocumentSet(Request.Data, &Result);
   COLrespondSuccess(Request.Address,Result);
}
