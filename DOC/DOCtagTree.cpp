// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCtagTree
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 28 August 2025 - 11:58AM
// ---------------------------------------------------------------------------

#include <DOC/DOCtagTree.h>
#include <DOC/DOCtag.h>
#include <DOC/DOCdocument.h>
#include <DOC/DOCserialize.h>

#include <FIL/FILutils.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLsplit.h>
#include <COL/COLarray.h>
#include <COL/COLvarUtils.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void DOCexploreTagNode(const COLstring& TagDir, COLvar* pResult){
   COL_FUNCTION(DOCexploreTagNode);
   COL_VAR(TagDir);
   COLvar& Result = *pResult;
   COLvar& Tags = Result["tags"];
   COLvar& Docs = Result["docs"];
   Tags.setArrayType();
   Docs.setArrayType();
   FILdirEnumerator Enum(TagDir+FIL_ANYFILE);
   COLstring FileName;
   while (Enum.getFile(FileName)){
      COL_VAR(FileName);
      if (Enum.isDirectory()){
         COLvar TagItem;
         TagItem["name"] = FileName;
         Tags.push_back(TagItem);
      } else {
         // We have file -- TODO - helper function
         COLstring Content;
         if(FILpathExtLast(FileName,true) == ".info"){
            FILreadFile(TagDir+FileName, &Content);
            DOCdocument Doc;
            DOCfromJson(Content, &Doc);
            COL_VAR(Doc);
            DOCtoVar(Doc, &Docs.push_back());
         }
      }
   }
}

COLvar* DOCtagFindChild(COLvar* pParent, const COLstring& TagName){
   COL_FUNCTION(DOCtagFindChild);
   COL_VAR2(TagName, *pParent);
   COLvar& Tags = (*pParent)["tags"];
   for (auto i=Tags.array().begin(); i != Tags.array().end(); i++){
      if ((*i)["name"].asString() == TagName){
         return i;
      }
   }
   return NULL;
}

bool DOCiterateTree(int i, const COLarray<COLstring>& Tags, COLvar* pOut, COLstring* pDir){
   COL_FUNCTION(DOCiterateTree);
   const COLstring Tag = Tags[i];
   *pDir += Tag + "/";
   if (!FILfileExists(*pDir)){
      return false;
   }
   COL_VAR(*pDir);
   DOCexploreTagNode(*pDir, pOut);
   i++;
   if (i < Tags.size()){
      COLvar* pChild = DOCtagFindChild(pOut, Tags[i]);
      if (!pChild){
         return false; // non existent tag
      }
      (*pChild)["children"].setArrayType();
      return DOCiterateTree(i, Tags, &(*pChild)["children"], pDir);
   }
   return true;
}

void DOCtagLoadFlatTags(const COLstring& Dir, COLvar* pFlatTags){
   COL_FUNCTION(DOCtagLoadFlatTags);
   COLvar& FlatTags = *pFlatTags;
   FlatTags.setArrayType();
   COLstring FileName = Dir + "tags.json";
   if (FILfileExists(FileName)){
      COLstring Content;
      FILreadFile(FileName, &Content);
      FlatTags.parse(Content);
      COL_TRC("Loaded tags file." << FlatTags);
   }
}

void DOCtagTree(const COLwebRequest& Request){
   COL_VAR(Request.Data.json(1));
   const COLvar& Data = Request.Data;

   COLstring Tag = COLvarRequiredString(Data, "tag");
   COLstring ComponentId  = COLvarRequiredString(Data, "component");

   COLarray<COLstring> Tags;
   COLsplit(&Tags, Tag, "_");
   Tags.insert("", 0);
   COLvar Result;
   COLstring Dir = DOCtagIndexDir(ComponentId);
   DOCtagLoadFlatTags(Dir, &Result["flattags"]);
   
   FILremovePathSeparator(Dir);
 
   bool Success = DOCiterateTree(0, Tags, &Result, &Dir);
   if (!Success){
      COL_TRC("Tag does not exist");
      return COLrespondError(Request.Address, "Tag does not exist.");
   }
   COL_VAR(Result.json(1));
   COLrespondSuccess(Request.Address, Result);
}