// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMutils
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 07 November 2023 - 04:15PM
// ---------------------------------------------------------------------------

#include <DBIM/DBIMutils.h>

#include <DB/DBgrammar.h>
#include <DB/DBgrammarLoad.h>

#include <CFG/CFGconfigApi.h>
#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool DBIMresolveVdbFile(const COLstring& Vdb, const COLstring& Component, COLstring* pVdbFullPath){
   COL_FUNCTION(DBIMresolveVdbFile);
   COLstring Filepath = Vdb;
   FILcorrectPathSeparators(Filepath);
   COLstring VdbFile = CFGresolveRepoFile(Component, Filepath, true);
   COL_TRC("Resolved vdb file: " + VdbFile);
   if (!FILfileExists(VdbFile) || FILisDirectory(VdbFile)){
      COL_TRC("Resolved file does not exist or is directory");
      return false;
   }
   *pVdbFullPath = VdbFile;
   return true;
}

bool DBIMreadVdbToDbCollection(const COLstring& Filepath, const COLstring& Component, COLstring* pResolvedVdb, DBcollection* pOut, COLstring* pError){
   COL_FUNCTION(DBIMreadVdbToDbCollection);
   if (!DBIMresolveVdbFile(Filepath, Component, pResolvedVdb)){
      COL_TRC("vdb file not found");
      *pError = "The specified vdb file was not found.";
      return false;
   }
   COL_TRC("Loading vdb file: " + *pResolvedVdb);
   COLstring VdbData;
   FILreadFile(*pResolvedVdb, &VdbData);
   COLstring DummyError; // won't report this
   if(!DBload(VdbData, pOut, &DummyError)) {
      COL_TRC("Error building grammar from file");
      *pError = "Could not load the content - the vdb file is incorrectly formatted.";
      return false;
   }
   return true;
}