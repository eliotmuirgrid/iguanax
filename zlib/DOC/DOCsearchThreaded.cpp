// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCsearchThreaded
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 07 October 2025 - 01:35PM
// ---------------------------------------------------------------------------

#include <DOC/DOCsearchThreaded.h>
#include <DOC/DOChex.h>
#include <DOC/DOCdir.h>
#include <DOC/DOCserialize.h>
#include <DOC/DOCdocument.h>

#include <FIL/FILutils.h>

#include <MATCH/MATCHsearch.h>

#include <COL/COLvarUtils.h>
#include <COL/COLmap.h>
#include <COL/COLlist.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring DOCstripExt(const COLstring& Name) {
    int pos = Name.find('.');
    if (pos < 0) return Name;
    return Name.substr(0, pos);
}

static bool DOCfileMatchesId(const COLstring& fileName, const COLstring& id) {
    if (id == "-1") return true;
    if (fileName == id) return true;
    if (fileName == (id + ".json")) return true;
    return DOCstripExt(fileName) == id;
}

bool DOCsearchLeaf(const COLstring& Dir, bool SearchForward, const COLstring& StartId,  bool* pIdConsumed,  COLclosure2<COLstring, bool*>* pCallback){
    COL_FUNCTION(DOCsearchLeaf);
    COLmap<COLstring, bool> FileList;
    FILdirList(&FileList, Dir + "*.json");
    bool KeepGoing = true;
    COL_VAR(Dir);

    const bool HasSpecificId = (StartId != "-1");
    // If no specific id or we already consumed it earlier, start immediately.
    bool Started = (!HasSpecificId) || (pIdConsumed && *pIdConsumed);

    auto MaybeStartHere = [&](const COLstring& Fname) {
        if (!Started && HasSpecificId && DOCfileMatchesId(Fname, StartId)) {
            Started = true;
            if (pIdConsumed) *pIdConsumed = true;
        }
        return Started;
    };

    if (SearchForward) {
        for (auto it = FileList.begin(); it != FileList.end(); ++it) {
            const COLstring& Fname = it->first;
            COL_VAR(Fname);
            if (!MaybeStartHere(Fname)) continue;

            pCallback->run(Dir + Fname, &KeepGoing);
            if (!KeepGoing) return false;
        }
    } else {
        for (auto it = FileList.last(); it != NULL; it = FileList.previous(it)) {
            COLstring Fname = FileList.key(it);
            if (!MaybeStartHere(Fname)) continue;

            pCallback->run(Dir + Fname, &KeepGoing);
            if (!KeepGoing) return false;
        }
    }
    return true;
}

bool DOCsearchDir(const COLstring& Dir, const COLstring& From, bool SearchForward, const COLstring& StartId, bool* pIdConsumed, COLclosure2<COLstring, bool*>* pCallback){
    COL_FUNCTION(DOCsearchDir);
    if (From.size() == 0){
        COL_TRC("Leaf");
        return DOCsearchLeaf(Dir, SearchForward, StartId, pIdConsumed, pCallback);
    }

    COLmap<COLstring, bool> FileList;
    FILdirList(&FileList, Dir + "*");

    COLstring Part = From.substr(0, 2);
    COLstring Remainder = From.substr(2);
    COL_VAR2(Part, Remainder);

    if (SearchForward){
        for (auto i = FileList.begin(); i != FileList.end(); ++i){
            COL_VAR2(i->first, i->second);
            if (!i->second) continue;            
            if (i->first.size() != 2) continue;    
            const COLstring& Name = i->first;

            if (Name < Part) continue;

            if (Name == Part) {
                if (!DOCsearchDir(Dir + Name + "/", Remainder, SearchForward, StartId, pIdConsumed, pCallback))
                    return false;
            } else {
                COLstring Cleared(Remainder.size(), '0');
                if (!DOCsearchDir(Dir + Name + "/", Cleared, SearchForward, StartId, pIdConsumed, pCallback))
                    return false;
            }
        }
    } else {
        for (COLavlTreePlace it = FileList.last(); it != NULL; it = FileList.previous(it)){
            COLstring Name = FileList.key(it);
            bool IsDir = FileList[it];

            if (!IsDir) continue;
            if (Name.size() != 2) continue;
            if (Name > Part) continue;

            if (Name == Part) {
                if (!DOCsearchDir(Dir + Name + "/", Remainder, SearchForward, StartId, pIdConsumed, pCallback))
                    return false;
            } else {
                COLstring Cleared(Remainder.size(), 'f');
                if (!DOCsearchDir(Dir + Name + "/", Cleared, SearchForward, StartId, pIdConsumed, pCallback))
                    return false;
            }
        }
    }
    return true;
}

static void DOCresultClosure(COLstring FileName, bool* pKeepGoing, COLstring Search, COLvar* pResult, int* pCount){
   COL_FUNCTION(DOCresultClosure);
   COLstring Content;
   FILreadFile(FileName, &Content);
   DOCdocument Doc;
   DOCfromJson(Content,&Doc);
   DOCdocumentCompileTagSearch(&Doc);

   if (MATCHsearch(Search,Doc.TagSearch,false,1)){
      COLvar Row;
      Row["id"] = Doc.Id;
      Row["created"] = DOCtoHex(Doc.Created);
      Row["title"] = Doc.Title;
      pResult->push_back(Row);
      (*pCount)--;
      if (0 == *pCount)
      {
         *pKeepGoing = false;
      }
   }
      
}

void DOCsearchDirectionally(const COLstring& Start, const COLstring& Component, bool SearchForward, const COLstring& Id,   COLclosure2<COLstring, bool*>* pCallback){
   COL_FUNCTION(DOCsearchDirectionally);
   COLstring Dir = DOCrepositoryDir(Component);
   COLstring StartPadded = DOCpaddHex(Start);
   StartPadded = StartPadded.substr(2);

   bool IdConsumed = false;
   DOCsearchDir(Dir, StartPadded, SearchForward, Id, &IdConsumed, pCallback);
}

void DOCwebSearchDirectionally(const COLwebRequest& Request){
   COL_FUNCTION(DOCwebSearchDirectionally);

   COLvar Data         = Request.Data;
   COLstring Start     = COLvarRequiredString(Data, "start");
   COLstring Component = COLvarRequiredString(Data, "component");
   int Limit           = COLvarRequiredInt(Data,"limit");
   bool SearchForward  = COLvarRequiredBool(Data,"search_forward");
   COLstring Search    = Data.exists("search") ? Data["search"] : "";
   COLstring Dir       = DOCrepositoryDir(Component);
   COLstring Id = (Data.exists("id") && Data["id"].asString() != "") ? Data["id"] : "-1";
   //Def a better way to do this
   Start = Start.substr(0,Start.size()-2);
   Start = DOCpaddHex(Start);
   Start = Start.substr(2);

   bool IdConsumed = false;
   COLvar Result; 

   DOCsearchDir(Dir, Start, SearchForward, Id, &IdConsumed, COLnewClosure2(&DOCresultClosure, Search, &Result, &Limit));
   
   COLrespondSuccess(Request.Address, Result);
}