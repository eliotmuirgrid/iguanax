// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCserialize
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday 19 August 2025 - 09:27AM
// ---------------------------------------------------------------------------

#include <DOC/DOCserialize.h>
#include <DOC/DOCdocument.h>
#include <DOC/DOChex.h>
#include <DOC/DOCdir.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLostream.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>

#include <stdlib.h>
COL_LOG_MODULE;

static void DOCrelatedToJson(const COLarray<DOCrelation>& Relations, COLvar* pOut){
   COL_FUNCTION(DOCrelatedToJson);
   COLvar& Out = *pOut;
   Out.setArrayType();
   for (int i = 0; i < Relations.size(); i++){
      COLvar R;
      R["id"]       = Relations[i].Id;
      R["relation"] = Relations[i].Relation;
      R["created"]  = Relations[i].Created;
      R["title"]    = Relations[i].Title;
      Out.push_back(R);
   }
}

void DOCrelatedFromJson(const COLvar& In, COLarray<DOCrelation>* pRelations){
   COL_FUNCTION(DOCrelatedFromJson);
   if (!In.isArray()){
      return;
   }
   COLvar::COLvarVec RelList = In.array();
   for (int i = 0; i < RelList.size(); i++){
      COLvar &RIn = RelList[i];
      DOCrelation R;
      R.Id = RIn["id"];
      R.Relation = RIn["relation"];
      R.Created  = RIn["created"];
      R.Title    = RIn["title"];
      pRelations->push_back(R);
   }
}

void DOCtagsToJson(const DOCdocument& Doc, COLvar* pOut){
   COL_FUNCTION(DOCtagsToJson);
   COLvar& Out = *pOut;
   Out.setArrayType();
   for (auto i=Doc.Tags.cbegin(); i != Doc.Tags.cend(); i++){
      Out.push_back(i->first);
   } 
}

void DOCtagsFromJson(const COLvar& In, DOCdocument* pDoc){
   COL_FUNCTION(DOCtagsFromJson);
   COLvar::COLvarVec List = In.array();
   for (int i = 0; i < List.size(); i++){
      pDoc->addTag(List[i]);
   } 
}

void DOCtoJson(const DOCdocument& Doc, COLstring *pOut){
   COL_FUNCTION(DOCtoJson);
   COLvar Data;
   DOCtoVar(Doc, &Data);
   *pOut = Data.json(1);
   COL_VAR(*pOut);
   return;
}

void DOCfromJson(const COLstring& In, DOCdocument* pOut){
   COL_FUNCTION(DOCfromJson);
   COLvar J;
   J.parse(In);
   DOCfromVar(J, pOut);
}

void DOCtoVar(const DOCdocument& Doc, COLvar* pOut){
   COL_FUNCTION(DOCtoVar);
   COLvar& J = *pOut;
   J["id"] = Doc.Id;
   J["created"] = DOCtoHex(Doc.Created);
   J["source"]  = Doc.Source;
   J["title"]   = Doc.Title;
   DOCrelatedToJson (Doc.Relations,    &J["relations"]);
   DOCtagsToJson    (Doc,              &J["tags"]);
   COL_VAR(Doc.Relations.size());
}

// PLEASE KEEP DOCtoVar and DOCfromVar close to each other so we
// can visually inspect that they are symmetric

//TODO add handling for required and optional DOC fields
// Older versions could cause issues if required/optional fields are not handled correctly

void DOCfromVar(const COLvar& In, DOCdocument* pOut){
   COL_FUNCTION(DOCfromVar);
   DOCdocument& Out = *pOut;
   Out.Id      = In["id"];
   Out.Source  = In["source"];
   Out.Data    = In["data"];
   Out.Title   = In["title"];
   Out.Created = DOChexStringToInt(In["created"]);
   if(In.exists("relations"))   {DOCrelatedFromJson (In["relations"],    &Out.Relations);};
   if(In.exists("tags"))        {DOCtagsFromJson    (In["tags"],         &Out);};

   if (Out.Source == "email"){
      COL_ERR("Fix for legacy repos.");
      Out.Source = "eml";
   }
}

static void DOCsaveData(const COLstring& FileName, const COLstring& Content){
   COL_FUNCTION(DOCsaveData);
   FILmakeFullDir(FILpathDir(FileName, true), FIL_USER_RWX, NULL);
   FILwriteFile(FileName, Content);
}

void DOCsaveDoc(const DOCdocument& Doc, const COLstring& Component){
   COL_FUNCTION(DOCsaveDoc);
   COLstring FileName = DOCfileMeta(Component, Doc);
   COL_VAR2(Component, FileName);
   COLstring Content;
   DOCtoJson(Doc, &Content);
   DOCsaveData(FileName,Content);
}

void DOCsaveOrig(const COLstring& Data, const DOCdocument& Doc, const COLstring& Id){
   COL_FUNCTION(DOCsaveOrig);
   DOCsaveData(DOCfileOriginal(Id, Doc),Data);
}

void DOCsaveMd(const COLstring& Data, const DOCdocument& Doc, const COLstring& Id){
   COL_FUNCTION(DOCsaveMd);
   DOCsaveData(DOCfileMd(Id, Doc),Data);
}

void DOCreadDoc(const COLstring& ComponentId, const COLstring& Id, const COLstring& CreatedHex, DOCdocument* pDoc){
   COL_FUNCTION(DOCreadDoc);
   DOCdocument Doc;
   Doc.Created = DOChexStringToInt(CreatedHex);
   Doc.Id      = Id;
   COLstring FileName = DOCfileMeta(ComponentId, Doc);
   COL_VAR2(FileName, Doc);
   COLstring FileData;
   FILreadFile(FileName, &FileData);
   DOCfromJson(FileData, pDoc);
}
