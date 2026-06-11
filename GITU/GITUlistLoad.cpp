// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUlistLoad
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 31 March 2023 - 10:21AM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILatomic.h>
#include <FIL/FILutils.h>
#include <GITU/GITUlist.h>
#include <GITU/GITUlistLoad.h>
#include <GITU/GITUurl.h>
#include <INI/INIdata.h>
COL_LOG_MODULE;

void GITUlistTemplateMap(const COLstring& SourceGitUrl, const COLvar& Data, GITUlist* pList){
   COL_FUNCTION(GITUlistTemplateMap);
   if(!Data.isMap()) { return; }
   auto Map = Data.map();
   for (auto i=Map.cbegin(); i != Map.cend(); i++){
      COLstring SshUrl;
      GITUsshUrl(i->first, &SshUrl);
      pList->List.push_back(GITUlistComponent(i->second["name"], i->second["description"], SshUrl, SourceGitUrl, i->second["branch"]));
   }
}

void GITUlistLibraryMap(const COLstring& SourceGitUrl, const COLvar& Data, GITUlist* pList){
   COL_FUNCTION(GITUlistLibraryMap);
   if(!Data.isMap()) { return; }
   auto Map = Data.map();
   for (auto i=Map.cbegin(); i != Map.cend(); i++){
      COLstring GitUrl; 
      GITUsshUrl(i->second["repo"], &GitUrl);
      COL_VAR3(i->second["name"], i->first, SourceGitUrl);
      pList->List.push_back(GITUlistLibrary(i->second["name"], i->second["description"], GitUrl, SourceGitUrl, i->second["branch"]));
   }
}

bool GITUlistMap(const COLstring& SourceGitUrl, const COLvar& Data, GITUlist* pList, COLstring* pError){
   COL_FUNCTION(GITUlistMap);
   if(!Data.isMap()) { return true; }
   auto Map = Data.map();
   for (auto i=Map.cbegin(); i != Map.cend(); i++){
      COLstring SourceUrl;
      COLstring GitUrl = i->second["git"];
      if(!GITUsshUrl(GitUrl, &SourceUrl)){
         COL_TRC("Could not parse: " << GitUrl);
         continue;
      }
      GITUrepoInfo GitRepo;
      bool Library   = i->second["type"].asString() == GITUrepoTypeString(GIT_LIBRARY);
      bool Component = i->second["type"].asString() == GITUrepoTypeString(GIT_COMPONENT);
      COL_VAR(Library);
      if(Library)  { GitRepo = GITUlistLibrary  (i->second["name"], i->second["description"], SourceUrl, SourceGitUrl, i->second["branch"]); }
      if(Component){ GitRepo = GITUlistComponent(i->second["name"], i->second["description"], SourceUrl, SourceGitUrl, i->second["branch"]); }
      if(Library || Component) pList->List.push_back(GitRepo);
   }
   return true;
}

bool GITUlistCollectionMap(const COLstring& SourceGitUrl, const COLvar& Data, GITUlist* pList, COLstring* pError){
   COL_FUNCTION(GITUlistCollectionMap);
   // OLD way
   if (Data.exists("template"))  { GITUlistTemplateMap(SourceGitUrl, Data["template"],   pList);}
   if (Data.exists("library"))   { GITUlistLibraryMap (SourceGitUrl, Data["library"] ,   pList);}
   // NEW way 
   if (Data.exists("collection")){ return GITUlistMap(SourceGitUrl, Data["collection"], pList, pError); }
   return true;
}

bool GITUlistCollectionLoad(const COLstring& SourceGitUrl, const COLstring& FileName, GITUlist* pList, COLstring* pError) {
   COL_FUNCTION(GITUlistCollectionLoad);
   COLstring Content, SshUrl;
   if(!GITUsshUrl(SourceGitUrl, &SshUrl)) {
      *pError = "Could not parse url: " + SourceGitUrl;
      COL_TRC("Could not parse url: " << SourceGitUrl);
      return false;
   }
   COL_VAR(FileName);
   try {
      FILreadFile(FileName, &Content);
      COLvar Data;
      Data.parse(Content);
      COL_VAR(Data.json(1));
      return GITUlistCollectionMap(SshUrl, Data, pList, pError);
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      *pError = E.description();
      return false;
   }
}

void GITUlistGitModuleMap(const COLstring& SourceGitUrl, const INIdata& Data, GITUlist* pList){
   COL_FUNCTION(GITUlistGitModuleMap);
   for (const auto& i : Data.PrivateData){
      COL_VAR(i.second);
      if(!i.second.count("path") || !i.second.count("url")){
         COL_TRC("No path or the git url. Not adding to the list.");
         continue;
      }
      const COLstring Branch = i.second.count("branch") == 0 ? "" : i.second["branch"];
      COLstring SshUrl;
      GITUsshUrl(i.second["url"], &SshUrl);
      pList->List.push_back(GITUlistModule(i.second["path"], SshUrl, SourceGitUrl, Branch));
   }
}

void GITUlistGitModuleLoad(const COLstring& SourceGitUrl, const COLstring& FileName, GITUlist* pList) {
   COL_FUNCTION(GITUlistGitModuleLoad);
   COLstring Content, SshUrl;
   COL_VAR(FileName);
   try {
      FILreadFile(FileName, &Content);
      INIdata Data;
      INIparse(Content, &Data);
      COL_VAR(Data);
      GITUsshUrl(SourceGitUrl, &SshUrl);
      GITUlistGitModuleMap(SshUrl, Data, pList);
   } catch(const COLerror& E) { COL_TRC(E.description()); }
}

void GITUlistCatalogueMap(const COLvar& Data, GITUlist* pList){
   COL_FUNCTION(GITUlistCatalogueMap);
   COLstring Username;
   GITUlistCatalogueMap(Data, Username, false, pList);
}

void GITUlistCatalogueMap(const COLvar& Data, const COLstring Username, bool CheckUrls, GITUlist* pList){
   COL_FUNCTION(GITUlistCatalogueMap);
   if(!Data.isMap()) { return; }
   for (const auto& i : Data.map()){
      COLstring Url;
      // do a parse http(s) depending on whether config is secure : true/false
      COLstring GitUrl = i->first;
      if(!GITUsshUrl(GitUrl, &Url)){
         COL_TRC("Could not parse url: " << GitUrl);
         continue;
      };
      GITUrepoInfo GitRepo;
      GitRepo.GitUrl = Url;
      pList->List.push_back(GitRepo);
      COL_VAR(Url);
   }
}


static void GITUlistDefaultCatalogue(GITUlist* pList){
   COL_FUNCTION(GITUlistCatalogueLoad);
   GITUrepoInfo GitRepo;
   GitRepo.GitUrl = "ssh://git@bitbucket.org/interfaceware/igx-api-collection.git";
   pList->List.push_back(GitRepo);
   GitRepo.GitUrl = "ssh://git@bitbucket.org/interfaceware/igx-business-collection.git";
   pList->List.push_back(GitRepo);
   GitRepo.GitUrl = "ssh://git@bitbucket.org/interfaceware/igx-core-collection.git";
   pList->List.push_back(GitRepo);
   GitRepo.GitUrl = "ssh://git@bitbucket.org/interfaceware/igx-data-transformation.git";
   pList->List.push_back(GitRepo);
   GitRepo.GitUrl = "ssh://git@bitbucket.org/interfaceware/igx-data-validation-collection.git";
   pList->List.push_back(GitRepo);
   GitRepo.GitUrl = "ssh://git@bitbucket.org/interfaceware/igx-devops-collection.git";
   pList->List.push_back(GitRepo);
   GitRepo.GitUrl = "ssh://git@bitbucket.org/interfaceware/igx-devtools-collection.git";
   pList->List.push_back(GitRepo);
   GitRepo.GitUrl = "ssh://git@bitbucket.org/interfaceware/igx-emr-collection.git";
   pList->List.push_back(GitRepo);
   GitRepo.GitUrl = "ssh://git@bitbucket.org/interfaceware/igx-notification-collection.git";
   pList->List.push_back(GitRepo);
   GitRepo.GitUrl = "ssh://git@bitbucket.org/interfaceware/igx-orchestration-collection.git";
   pList->List.push_back(GitRepo);
   GitRepo.GitUrl = "ssh://git@bitbucket.org/interfaceware/igx-security-collection.git";
   pList->List.push_back(GitRepo);
   GitRepo.GitUrl = "ssh://git@bitbucket.org/interfaceware/igx-storage-collection.git";
   pList->List.push_back(GitRepo);
}

void GITUdefaultCatalogueOnStartUp(){
   COL_FUNCTION(GITUdefaultCatalogueOnStartUp);
   COLstring CatalogueFileName = DIRfileConfigGitCatalogue();
   if(FILfileExists(CatalogueFileName)) {
      COL_TRC("Catalogue file already exists!");
      return;
   }
   GITUlist List;
   GITUlistDefaultCatalogue(&List);
   GITUlistCatalogueSave(List);
}

bool GITUlistCatalogueLoad(GITUlist* pList) {
   COL_FUNCTION(GITUlistCatalogueLoad);
   const COLstring FileName = DIRfileConfigGitCatalogue();
   if(!FILfileExists(FileName)) { return false; }
   try {
      COLstring Content;
      FILreadFile(FileName, &Content);
      COLvar Data;
      Data.parse(Content);
      GITUlistCatalogueMap(Data, pList);
      return true;
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      return false;
   }
}

static void GITUlistToCatalogueVar(const GITUlist& List, COLvar* pData) {
   COL_FUNCTION(GITUlistToCatalogueVar);
   COLvar& Data = *pData;
   Data.setMapType();
   for(int i = 0; i < List.List.size(); i++) { Data[List.List[i].GitUrl].setMapType(); }
   COL_VAR(Data.json(1));
}

void GITUlistCatalogueSave(const GITUlist& List) {
   COL_FUNCTION(GITUlistCatalogueSave);
   COLvar Data;
   GITUlistToCatalogueVar(List, &Data);
   COLstring  Error;
   const bool Success = FILsave(DIRfileConfigGitCatalogue(), Data.json(1), &Error);
   COL_VAR2(Success, Error);
}

bool GITUlistLoad(const COLstring& iDir, const COLstring& SourceGitUrl, GITUlist* pList){
   COL_FUNCTION(GITUlistLoad);
   COLstring Dir(iDir);
   COL_VAR(iDir);
   FILaddPathSeparator(Dir);
   COL_VAR(Dir);
   bool Success = false;
   COLstring Error;
   if (FILfileExists(Dir + "collection.json")){
      GITUlistCollectionLoad(SourceGitUrl, Dir + "collection.json", pList, &Error);
      Success = true;
   }
   if (FILfileExists(Dir + ".gitmodules")){
      GITUlistGitModuleLoad(SourceGitUrl, Dir + ".gitmodules", pList);
      Success = true;
   }
   return Success;
}


