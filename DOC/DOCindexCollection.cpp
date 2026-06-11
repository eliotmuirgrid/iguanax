// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCindexCollection
//
// Description:
//
// Implementation
//
// Author: Stephen Piccone
// Date:   Monday 25 August 2025 - 10:00AM
// ---------------------------------------------------------------------------

#include <DOC/DOCindexCollection.h>
#include <DOC/DOCdocument.h>
#include <DOC/DOCserialize.h>
#include <DOC/DOCtag.h>
#include <DOC/DOCdir.h>
#include <DOC/DOCsearch.h>

#include <MATCH/MATCHsearch.h>

#include <FIL/FILdirEnumerator.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <DIR/DIRpath.h>

#include <COL/COLlist.h>
#include <COL/COLweb.h>
#include <COL/COLvarUtils.h>
#include <COL/COLsplit.h>
#include <COL/COLmap.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

class DOCdir{
public:
   COLhashmap<COLstring, DOCdir> SubDirs;
};

static bool DOCguidExists(COLstring& Guid){
   COLstring Path = DIRcomponentDevDir(Guid);
   return FILfileExists(Path);
}

// recursive operator<<
COLostream& operator<<(COLostream& Stream, const DOCdir& Dir){
   Stream << newline;
   for (auto i = Dir.SubDirs.cbegin(); i != Dir.SubDirs.cend(); ++i){
      const COLstring& name = i.key();
      const DOCdir& child = i->second;
      Stream << name << " {" << indent << indent;
         Stream << child;
      Stream << "}" << unindent << unindent << newline;
   }
   return Stream;
}


bool DOCindexTagIsMultiLevel(const COLstring& Tag){
   COL_FUNCTION(DOCindexTagIsMultiLevel);
   return Tag.find("_") != npos;
}

void DOCindexFlatTags(const COLlist<DOCdocument> &List, COLmap<COLstring,int>* pFlatTags){
   COL_FUNCTION(DOCindexFlatTags);
   for (auto i = List.cbegin(); i != List.cend(); i++){
      const DOCdocument &Doc = *i;
      for (auto TagIter = Doc.Tags.cbegin(); TagIter != Doc.Tags.cend(); ++TagIter){
         const COLstring& Tag = TagIter->first;
         if (!DOCindexTagIsMultiLevel(Tag)){
            if (!pFlatTags->find(Tag)){
               pFlatTags->add(Tag, 0);
            }
         }
      }
   }
}

// This function will generate a complete list of all the directories that need to be created.
void DOCcatalogDirs(const COLlist<DOCdocument> &List, DOCdir* pDirectory){
   for (auto i = List.cbegin(); i != List.cend(); i++){
      const DOCdocument &Doc = *i;
      for (auto TagIter = Doc.Tags.cbegin(); TagIter != Doc.Tags.cend(); ++TagIter){
         const COLstring& Tag = TagIter->first;
         COL_VAR(Tag);
         if (DOCindexTagIsMultiLevel(Tag)){
            COLarray<COLstring> Parts;
            COLsplit(&Parts, Tag, "_");
            //Keep track of current dir
            DOCdir* pCurrentParent = pDirectory;
            for (size_t j = 0; j < Parts.size(); ++j){
               const COLstring &Part = Parts[j];
               pCurrentParent = &pCurrentParent->SubDirs[Part];
            }
         }
      }
   }
   COL_VAR(*pDirectory);
}

static void DOCcreateDirsHelper(const COLstring& Parent, const DOCdir& Children, const COLstring& RootDir){
   COLstring FolderPath = RootDir + Parent + FIL_DIR_SEPARATOR;
   FILmakeDir(RootDir + Parent + FIL_DIR_SEPARATOR, FIL_USER_RWX);
   for (auto i = Children.SubDirs.cbegin(); i != Children.SubDirs.cend(); ++i) {
      DOCcreateDirsHelper(i->first, i->second, FolderPath);
   }
}

void DOCcreateDirs(const DOCdir& Directory, const COLstring& RootDir){
   COL_FUNCTION(DOCcreateDirs);
   //Only to make the actual root dir itself
   FILmakeFullDir(RootDir,FIL_USER_RWX);
   for (auto i = Directory.SubDirs.cbegin(); i != Directory.SubDirs.cend(); i++){
      DOCcreateDirsHelper(i->first,i->second,RootDir);
   }   
}

void DOCwriteIndexFiles(const COLlist<DOCdocument>& DocList, const COLstring& RootDir){
   COL_FUNCTION(DOCwriteIndexFiles);
   COL_VAR(RootDir);
   for (auto i = DocList.cbegin(); i != DocList.cend(); i++){
      const DOCdocument &Doc = *i;
      for (auto j = Doc.Tags.cbegin(); j != Doc.Tags.cend(); ++j){
         const COLstring& Tag = j->first;
         //Logic from DOCtag, cant use the function because it has values hardcoded
         if (DOCindexTagIsMultiLevel(Tag)){
            COLstring Dir(Tag);
            Dir.strip(COLstring::Both, ' ');
            Dir.replace("_", "/");
            Dir+= FIL_DIR_SEPARATOR + Doc.Id + ".info";
            Dir = RootDir + Dir;
            COL_VAR(Dir);
            COLstring Docjson;
            DOCtoJson(Doc, &Docjson);
            FILwriteFile(Dir,Docjson);
         }
      }
   }
}

void DOCwriteFlatTags(const COLmap<COLstring, int>& Tags, const COLstring& Dir){
   COL_FUNCTION(DOCwriteFlatTags);
   COLvar Result;
   Result.setArrayType();
   for (auto i=Tags.cbegin(); i != Tags.cend(); i++){
      Result.push_back(i->first);
   }
   FILwriteFile(Dir+"tags.json", Result.json());
}

void DOCdocumentLoadMarkdown(const COLstring& ComponentId, DOCdocument* pDoc){
   COL_FUNCTION(DOCdocumentLoadMarkdown);
   COLstring FileName = DOCfileMd(ComponentId, *pDoc);
   if (FILfileExists(FileName)){
      FILreadFile(FileName, &pDoc->Markup);
   } else {
      COL_ERR("No markup file found.");
   }
}

void DOCdocumentListLoadMarkdown(const COLstring& ComponentId, COLlist<DOCdocument>* pDocList){
   COL_FUNCTION(DOCdocumentLoadMarkdown);
   for (auto i = pDocList->begin(); i != pDocList->end(); i++){
      DOCdocumentLoadMarkdown(ComponentId, &(*i));
   }
}

void DOCmatchList(const COLstring& Search, COLlist<DOCdocument>* pList){
   COL_FUNCTION(DOCmatchList);
   COL_VAR(Search);
   COLlist<DOCdocument>& L = *pList;
   COLlistPlace i = L.first();
   while (i != NULL){
      COLlistPlace C = i;
      i = L.next(i);
      if (!MATCHsearch(Search, L[C].TagSearch, false,1) 
       && !MATCHsearch(Search, L[C].Markup,    false,1)
       && !MATCHsearch(Search, L[C].Title,     false,1)){
         COL_TRC("Removing DOC");
         L.remove(C);       
      }
   }
}

void DOCindexCollection(const COLwebRequest& Request){
   COL_FUNCTION(DOCindexCollection);
   COLstring ComponentId = COLvarRequiredString(Request.Data, "component");
   if(!DOCguidExists(ComponentId)){
      return COLrespondError(Request.Address, "ComponentId does not exist.");
   }
   COLstring Search = COLvarGetString(Request.Data, "search", "");
   COLlist<DOCdocument> DocList;
   DOCdir Directory;
   // TODO have a flag to read in the markdown document for the documents based on the search type
   DOCsearchLoad(ComponentId, &DocList);
   DOCdocumentListCompileTagSearch(&DocList);

   // TODO - make this conditional on it being a 'full search'.
   DOCdocumentListLoadMarkdown(ComponentId, &DocList);
   COLmap<COLstring, int> FlatTags;
   COL_VAR(DocList.size() << " docs before search.");
   if (!Search.is_null()){
      DOCmatchList(Search, &DocList);
   }
   COL_VAR(DocList.size() << " docs after search.");
   DOCindexFlatTags(DocList, &FlatTags);
   DOCcatalogDirs(DocList, &Directory);
   COLstring TempDir = DOCrepositoryDir(ComponentId) + "index_temp/";
   DOCcreateDirs(Directory, TempDir);  // make the root dir index2
   DOCwriteIndexFiles(DocList, TempDir);
   // rename index to index_old 
   FILrenameDirectory(DOCrepositoryDir(ComponentId) + "index/",DOCrepositoryDir(ComponentId) + "index_old/",FIL_USER_RWX,true);
   // rename index2 to index
   DOCwriteFlatTags(FlatTags, DOCrepositoryDir(ComponentId) + "index_temp/");
   FILrenameDirectory(DOCrepositoryDir(ComponentId) + "index_temp/",DOCrepositoryDir(ComponentId) + "index/",FIL_USER_RWX,true);
   // delete index_old
   FILremoveFullDirNew(DOCrepositoryDir(ComponentId) + "index_old/");
   COLvar Result;


   COLrespondSuccess(Request.Address, Result);
}
