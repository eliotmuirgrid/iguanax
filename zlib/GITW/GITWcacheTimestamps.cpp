// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWcacheTimestamps
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Wednesday 03 December 2025 - 10:38AM
// ---------------------------------------------------------------------------
#include <GITW/GITWcacheTimestamps.h>
#include <FIL/FILutils.h>
#include <GIT/GITcommits.h>
#include <GIT/GITpath.h>
#include <GITU/GITUcollection.h>
#include <GITU/GITUcollectionLoad.h>
#include <GITU/GITUlist.h>
#include <GITU/GITUlistLoad.h>

#include <COL/COLarray.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// keeping the COL_VAR and COL_TRC commented out, this can be noisy
static void GITWcacheTimestampsPopulate(const COLvar& Data, COLvar* pOut){
   COL_FUNCTION(GITWcacheTimestampsPopulate);
   COLvar& Out = *pOut;
   for (const auto& Entry : Data.map()){
      COLvar Temp = Entry.second;
      if (!Temp.exists("git") || Temp["git"].asString().is_null())     { continue; }
      if (Temp.exists("type") && Temp["type"].asString() == "library") { continue; }
      const COLstring Repo = Temp["git"];
      const COLstring Dir = GITgitTemplateDir(Repo);
      // COL_VAR2(Repo, Dir);
      COLstring Timestamp, Error;
      if (!GITlastCommitTime(Dir, &Timestamp, &Error)) { COL_DBG("skipping repo " + Repo); continue; } 
      if (!Out.exists(Timestamp)) {
         // COL_TRC("Unique timestamp, add to output");
         Out[Timestamp].push_back(Repo); 
      }
      else if (Out[Timestamp].array().indexOf(Repo) == -1) {
         // COL_TRC("Timestamp exists but repo not yet in array, add to output");
         Out[Timestamp].push_back(Repo);
      }
   }
}


// /git/cache/timestamps
void GITWcacheTimestamps(const COLwebRequest& Request){
   COL_FUNCTION(GITWcacheTimestamps);
   if(!FILfileExists(GITcacheDir())) { 
      return COLrespondSuccess(Request.Address); // Early return if no cache 
   }
   GITUlist Catalog;
	if(!GITUlistCatalogueLoad(&Catalog)){ 
      return COLrespondError(Request.Address, "Failed to load catalogue");
   }
	GITUcollection Collection;
	GITUcollectionLoadFromCatalogue(Catalog, &Collection);
	COLvar Data = GITUcollectionItems(Collection, false);
   // COL_VAR(Data.json(1));
   COLvar Out; // map of timestamp -> [git url array]
   Out.setMapType();  
   GITWcacheTimestampsPopulate(Data, &Out);
   COLrespondSuccess(Request.Address, Out);
}
