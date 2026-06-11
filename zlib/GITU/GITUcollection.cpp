// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUcollection
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 13 February 2023 - 11:23AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <GITU/GITUcollection.h>
#include <GITU/GITUurl.h>
#include <GSV/GSVlink.h>
#include <GUID/GUIDid.h>
#include <GITU/GITUutils.h>
COL_LOG_MODULE;

GITUcollection::GITUcollection(){
   COL_METHOD(GITUcollection::GITUcollection);
}

GITUcollection::~GITUcollection(){
   COL_METHOD(GITUcollection::~GITUcollection);
}

COLostream& operator<<(COLostream& Stream, const GITUcollection& Collection){
   Stream << "Collection: " << Collection.m_Name <<  " (" << Collection.m_Description << ")" << newline;
   Stream << Collection.m_List.List.size() << " libraries " << newline;
   for(const auto& GitRepo : Collection.m_List.List) {
      Stream << "  " << GitRepo.GitUrl << "=[" << GitRepo.Name << ", " << GitRepo.Description << "]" << newline;
   }
   return Stream;
}

COLvar GITUcollectionItems(const GITUcollection& Collection, bool FrontEnd) {
   COL_FUNCTION(GITUcollectionItems);
   COLvar CollectionOutput;
   CollectionOutput.setMapType();
   for(int i = 0; i < Collection.m_List.List.size(); i++) {
      const auto& GitRepo = Collection.m_List.List[i];
      COLvar      Entry;
      COLstring   GitUrl;
      GITUsshUrl(GitRepo.GitUrl, &GitUrl);
      Entry["description"] = GitRepo.Description;
      Entry["name"]        = GitRepo.Name;
      Entry["git"]         = GitUrl;
      Entry["branch"]      = GitRepo.Branch;
      Entry["type"]        = GITUrepoTypeString(GitRepo.RepoType);
      if(FrontEnd) {
         COLstring CollectionUrl;
         GITUsshUrl(GitRepo.SourceGitUrl, &CollectionUrl);
         GSVlink Link;
         GSVconstructLink(GitUrl, &Link, GitRepo.Branch);
         Entry["urls"]       = GSVlinkToVar(Link);
         Entry["collection"] = CollectionUrl;
         GitUrl              = GitRepo.Name + COLint32ToString(i) + GitUrl;
      }
      const COLstring Key   = GITUresolveKeyFromBranchUrl(GitUrl, GitRepo.Branch);
      CollectionOutput[Key] = Entry;
   }
   return CollectionOutput;
}

void GITUcollectionVar(const GITUcollection& Collection, COLvar* pOut) {
   COL_FUNCTION(GITUcollectionVar);
   COLvar& Out = *pOut;
   Out["name"]        = Collection.m_Name;
   Out["description"] = Collection.m_Description;
   Out["collection"]  = GITUcollectionItems(Collection, false);
   COL_VAR(Out.json(1));
}

void GITUcollectionVarComponents(const GITUcollection& Collection, COLvar* pOut){
   COL_FUNCTION(GITUcollectionVarComponents);
   COLvar& Out = *pOut;
   int j = 0;
   for(const auto& GitRepo : Collection.m_List.List) {
      if(GitRepo.RepoType != GIT_COMPONENT) continue;
      COLvar Entry;
      Entry["description"] = GitRepo.Description;
      Entry["name"]        = GitRepo.Name;
      Entry["git"]         = GitRepo.GitUrl;
      Entry["collection"]  = GitRepo.SourceGitUrl;
      Out[j] = Entry; 
      j++;
   }
   COL_VAR(Out.json(1));
}


void GITUcollectionVarLibraries(const GITUcollection& Collection, COLvar* pOut){
   COL_FUNCTION(GITUcollectionVarLibraries);
   COLvar& Out = *pOut;
   int j = 0;
   for(const auto& GitRepo : Collection.m_List.List) {
      if(GitRepo.RepoType != GIT_LIBRARY) continue;
      COLvar Entry;
      Entry["description"] = GitRepo.Description;
      Entry["name"]        = GitRepo.Name;
      Entry["git"]         = GitRepo.GitUrl;
      Entry["collection"]  = GitRepo.SourceGitUrl;
      Out[j] = Entry; 
      j++;
   }
   COL_VAR(Out.json(1));
}

