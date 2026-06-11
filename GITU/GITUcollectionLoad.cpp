// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUcollectionLoad
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 13 February 2023 - 11:31AM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <FIL/FILutils.h>
#include <GITU/GITUcollection.h>
#include <GITU/GITUcollectionLoad.h>
#include <GITU/GITUlistLoad.h>
#include <GIT/GITpath.h>
#include <GITU/GITUurl.h>
COL_LOG_MODULE;

bool GITUcollectionLoad(GITUcollection* pCollection, const COLvar& Data, const COLstring& GitUrl, COLstring* pError){
   COL_FUNCTION(GITUcollectionLoad);
   GITUcollection& Collection = *pCollection;
   Collection.m_Name        = Data["name"];
   Collection.m_Description = Data["description"];
   COLstring SourceGitUrl;
   if(!GITUsshUrl(GitUrl, &SourceGitUrl)){
      *pError = "Could not parse url: " + GitUrl;
      COL_TRC("Could not parse url: " << GitUrl);
      return false;
   }
   COL_VAR(SourceGitUrl);
   return GITUlistCollectionMap(SourceGitUrl, Data, &(pCollection->m_List), pError);
}

bool GITUcollectionLoadFromUrl(GITUcollection* pCollection, const COLstring& GitUrl, COLstring* pError){
   COL_FUNCTION(GITUcollectionLoadFromUrl);
   COLstring File = GITUcollectionFileName(GitUrl);
   COLstring Content;
   if(!FILfileExists(File)){
      *pError = "Could not find: " + File;
      return false;
   }
   try {
      FILreadFile(File, &Content);
      COLvar Data;
      Data.parse(Content);
      return GITUcollectionLoad(pCollection, Data, GitUrl, pError);
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      *pError = E.description();
      return false;
   }
}

void GITUcollectionLoadFromCatalogue(const GITUlist& CatalogList, GITUcollection* pCollection){
   COL_FUNCTION(GITUcollectionLoadFromCatalogue);
   COLstring Error;
   for(int i = 0; i < CatalogList.List.size(); i++) {
      GITUcollectionLoadFromUrl(pCollection, CatalogList.List[i].GitUrl, &Error);
   }
}