// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCsearch
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 25 September 2025 - 01:40PM
// ---------------------------------------------------------------------------

#include <DOC/DOCsearch.h>
#include <DOC/DOCdir.h>
#include <DOC/DOCdocument.h>
#include <DOC/DOCserialize.h>
#include <DOC/DOChex.h>

#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAutils.h>


#include <MATCH/MATCHsearch.h>

#include <FIL/FILdirEnumerator.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>


#include <COL/COLlog.h>
COL_LOG_MODULE;

void DOCinhaleDocuments(const COLstring& Dir, COLlist<DOCdocument>* pList){
   COL_FUNCTION(DOCinhaleDocuments);
   COL_VAR(Dir);
   FILdirEnumerator Enum(Dir + FIL_ANYFILE);
   COLstring FileName;
   // Stop recursive when there are no more files
   while (Enum.getFile(FileName)){
      COLstring FilePath = Dir + FileName;
      // Its a dir, recursive call
      if (Enum.isDirectory()){
         DOCinhaleDocuments(FilePath + FIL_DIR_SEPARATOR, pList);
      }else {// It is a file, add it to list
         if (FILpathExtLast(FileName, false) == "json"){  // TODO bit fragile.
            DOCdocument Doc;
            COLstring FileData;
            FILreadFile(FilePath, &FileData);
            DOCfromJson(FileData, &Doc);
            pList->add(Doc);
         }
      }
   }
}

void DOCsearchLoad(const COLstring& ComponentId,  COLlist<DOCdocument>* pList){
   COL_FUNCTION(DOCsearchLoad);
   COLstring Dir = DOCrepositoryDir(ComponentId);
   DOCinhaleDocuments(Dir, pList);
}

void DOCdocumentListCompileTagSearch(COLlist<DOCdocument>* pDocList){
   COL_FUNCTION(DOCdocumentListCompileTagSearch);
   for (auto i = pDocList->begin(); i != pDocList->end(); i++){
      DOCdocumentCompileTagSearch(&(*i));
   }
}

void DOCsearchList(const COLstring& Search, COLlist<DOCdocument>* pList){
   COL_FUNCTION(DOCsearchList);
   COL_VAR(Search);
   COLlist<DOCdocument>& L = *pList;
   COLlistPlace i = L.first();
   while (i != NULL){
      COLlistPlace C = i;
      i = L.next(i);
      COL_VAR(L[C].TagSearch);
      if (!MATCHsearch(Search, L[C].TagSearch, false,1)){
         COL_TRC("Removing DOC");
         L.remove(C);       
      }
   }
}

void DOCsearchFormatResult(const COLlist<DOCdocument>& List, COLvar* pOut){
   COL_FUNCTION(DOCsearchFormatResult);
   COLvar& Out = *pOut;
   Out.setArrayType();
   int j=0;
   for (auto i = List.cbegin(); i != List.cend(); i++){
      Out[j].setMapType();
      Out[j]["id"]        = i->Id;
      Out[j]["created"]   = DOCtoHex(i->Created);
      Out[j]["title"]     = i->Title;
      j++;
   }
}

void DOCsearchFormatTags(const COLlist<DOCdocument>& List, COLvar* pOut){
   COL_FUNCTION(DOCsearchFormatTags);
   COLvar& Out = *pOut;
   COLmap<COLstring, int> TagMap;

   for (auto i = List.cbegin(); i != List.cend(); ++i) {
       const DOCdocument& Doc = *i;
       for (auto t = Doc.Tags.cbegin(); t != Doc.Tags.cend(); ++t) {
         TagMap[t->first] = 0; 
       }
   }
   
   Out.setArrayType();
     for (auto it = TagMap.cbegin(); it != TagMap.cend(); ++it) {
        const COLstring& Tag = it->first;
        Out.push_back(Tag); 
    }

}

void DOCsearch(const COLvar& Data, COLvar* pOut){
   COL_FUNCTION(DOCsearch);
   COLvar& Out = *pOut;
   COLstring Component = COLvarRequiredString(Data, "component");
   COLstring Search    = COLvarRequiredString(Data, "search"   );
   COLlist<DOCdocument> List;
   DOCsearchLoad(Component, &List);
   COL_TRC("Read " << List.size() << "docs");
   DOCdocumentListCompileTagSearch(&List);
   COL_VAR(Search);
   DOCsearchList(Search, &List);
   COL_TRC("After search " << List.size() << "docs");
   DOCsearchFormatResult(List, &Out["list"]);
   DOCsearchFormatTags(List, &Out["tags"]);
   Out["component"] = Component;
}

int DOCluaSearch(lua_State* L) {
   COL_FUNCTION(DOCluaSearch);
   COLvar Data;
   COLvar Result;
   Data.fromLua(L);
   try{
      DOCsearch(Data, &Result);
   }
   LUA_CATCH()
   Result.toLua(L);
   return 1;
}

void DOCwebDocumentSearch(const COLwebRequest& Request){
   COL_FUNCTION(DOCwebDocumentSearch);
   COL_VAR(Request.Data.json(1));
   COLvar Result;
   DOCsearch(Request.Data, &Result);
   COLrespondSuccess(Request.Address,Result);
}
