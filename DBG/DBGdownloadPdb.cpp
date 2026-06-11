// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBGdownloadPdb
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 13 August 2023 - 10:23AM
// ---------------------------------------------------------------------------

#include <DBG/DBGdownloadPdb.h>
#include <DBG/DBGpdb.h>

#include <CACHE/CACHEfile.h>

#include <DBDVER/DBDVERgitInfo.h>

#include <DIR/DIRurl.h>
#include <DIR/DIRpath.h>

#include <FIL/FILutils.h>

#include <FIL/FILpathUtils.h>

#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>

#include <GZIP/GZIPutils.h>

#include <COL/COLsimulateOs.h>

#include <COL/COLlist.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLarray<COLstring> DBGpdbSearchPaths() {
   COL_FUNCTION(DBGpdbSearchPaths);
   COLarray<COLstring> Paths;
   COLstring Temp, TempErr;
   Paths.push_back(DIRresourcesDir());                                     // working_dir/resources/
   FILpathSimplify(FILexecutableDir() + "..\\..\\", &Temp, &TempErr);      // main2/Iguana/
   Paths.push_back(Temp);     
   return Paths;
}

COLstring DBGpdbFileName(){
   COL_FUNCTION(DBGpdbFileName);
   COLstring PdbFile = COLstring(DBDVER_COMMIT_ID) + ".pdb";
   COL_VAR(PdbFile);
   return PdbFile;
}

COLstring DBGfindPdbFile(const COLstring& FileName, const COLstring& LastResult){
   COL_FUNCTION(DBGfindPdbFile);
   if (!LastResult.is_null()){
      return LastResult;
   }
   COLstring Name, NameErr;
   FILpathSimplify(FileName, &Name, &NameErr);
   COL_VAR(Name);
   if (FILfileExists(Name)){
      COL_TRC("Found it!");
      return Name;
   }
   COL_TRC("Didn't find it.");
   return "";
}

static COLstring DBGpdbLocation(const COLarray<COLstring>& PathList){
   COL_FUNCTION(DBGpdbLocation);
   COLstring PdbFile = DBGpdbFileName();
   COLstring Result;
   Result = DBGfindPdbFile(PdbFile, Result);
   for (int i = 0; i < PathList.size(); ++i) {
      Result = DBGfindPdbFile(FILpathAppend(PathList[i], PdbFile), Result);
      if (!Result.is_null()) { break; }
   }
   COL_VAR(Result);
   return Result;
}

static bool DBGfindPdb(const COLarray<COLstring>& PathList){
   COL_FUNCTION(DBGfindPdb);
   COLstring PdbFileLocation = DBGpdbLocation(PathList);
   if (PdbFileLocation.is_null()){
      COL_TRC("Could not find it.");
      return false;
   }
   return true;
}

static void DBGiguanaPdbSearchPathInit(const COLarray<COLstring>& PathList){
   COL_FUNCTION(DBGiguanaPdbSearchPathInit);
   COLstring SearchPath;
   for (int i = 0; i < PathList.size(); ++i) {
      SearchPath.append(PathList[i]);
      if (i < PathList.size() - 1) {
         SearchPath.append(";");
      }
   }
   COL_VAR(SearchPath);
   // If SearchPath is longer than 150 chars, SymInitialize() fails silently. See IX-3428
   COLASSERT(SearchPath.size() <= 150);
   COLsetDebugSearchPath(SearchPath);
}

bool DBGextractPDB(COLstring* pFeedback){
   COL_FUNCTION(DBGextractPDB);
   if (!CACHEexists()){
      *pFeedback = "No file cache so nothing to extract.";
      return false;
   }
   COLstring Content;
   bool Result = CACHEreadFile("/pdb/" + DBGpdbFileName(), &Content);
   if (!Result){
      *pFeedback = "PDB file not in cache.";
      return false;
   }
   COLstring Out;
   GZIPuncompress(Content, &Out);
   COLstring PdbDir = DIRresourcesDir();
   try {
      if (FILfileExists(PdbDir)){
         COLlist<COLstring> PdbList;
         FILdirList(&PdbList, PdbDir + "*pdb");
         for (auto i=PdbList.begin(); i != PdbList.end(); i++){
            COL_TRC("Remove " << *i);
            int ErrorCode = FILremove(PdbDir + *i);
            COL_VAR(ErrorCode);
         }
      }
   } catch (COLerror& Error){
      COL_ERR(Error.description());
   }
   if (!FILfileExists(PdbDir)){
      COL_TRC("Making  " << PdbDir);
      FILmakeDir(PdbDir, FIL_USER_RWX & FIL_GROUP_RX);
   }
   COL_TRC("Writing " << PdbDir + DBGpdbFileName() << " (" << Out.size() << " bytes)");
   FILwriteFile(PdbDir + DBGpdbFileName(), Out);
   return true;
}

// Actually extract PDB if needed
bool DBGextractPdbIfNeeded(COLstring* pFeedback){
   COL_FUNCTION(DBGextractPdbIfNeeded);
   COLarray<COLstring> Paths = DBGpdbSearchPaths();
   DBGiguanaPdbSearchPathInit(Paths);
   bool NeedDownload = !DBGfindPdb(Paths);
   if (NeedDownload){
      COL_TRC("Need to extract it");
      bool Success = DBGextractPDB(pFeedback);
      COL_VAR(*pFeedback);
      if (Success){
         COL_TRC("Extraction was successful so let's initialize debugging.");
      } else {
         return false;
      }
   }
   return true;
}

// /debug/download/pdb
void DBGpdbInfo(const COLwebRequest& Request){
   COL_FUNCTION(DBGpdbInfo);
   COLvar Result;
   COLstring PDB = DBGprogramDatabase();
   if (PDB.is_null()){
      PDB = "PDB not loaded";
   }
   Result["pdb"] = PDB;
   COLrespondSuccess(Request.Address, Result);
}
