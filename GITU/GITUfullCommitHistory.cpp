// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUfullCommitHistory
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Monday 29 July 2024 - 02:21PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITcommits.h>
#include <GITU/GITUfullCommitHistory.h>
#include <GITU/GITUlist.h>
#include <GITU/GITUurl.h>
#include <INI/INIdata.h>
COL_LOG_MODULE;

void GITUgetFullFilePath(const COLstring& Dir, const COLstring& RelPath, COLvar* pResult, COLstring* pError) {
   COL_FUNCTION(GITUgetFullFilePath);
   GITUlist List;
   (*pResult)[RelPath]      = true;
   const COLstring DirPath  = FILpathAppend(Dir, RelPath);
   const COLstring FileName = DirPath + ".gitmodules";
   if(!FILfileExists(FileName)) { return; }
   try {
      COLstring Content;
      COL_VAR(FileName);
      FILreadFile(FileName, &Content);
      INIdata Data;
      INIparse(Content, &Data);
      COL_VAR(Data);
      for(auto i = Data.PrivateData.cbegin(); i != Data.PrivateData.cend(); i++) {
         COL_VAR(i->second);
         if(!i->second.count("path")) {
            COL_TRC("No path.");
            continue;
         }
         GITUgetFullFilePath(Dir, i->second["path"], pResult, pError);
      }
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      *pError = E.description();
   }
}

bool GITUfullCommitHistory(const COLstring& Dir, bool Development, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(GITUfullCommitHistory);
   COLstring& Error = *pError;
   COLvar& Result = *pResult;
   COLvar FilePathList;
   Result.setMapType();
   FilePathList.setMapType();
   GITUgetFullFilePath(Dir, "", &FilePathList, pError); // Get the full list of file paths to check
   COL_VAR(FilePathList);
   for (auto i = FilePathList.map().begin(); i != FilePathList.map().end(); ++i){
      COLstring Path = i->first;
      COLstring Content, Error;
      COLstring FullPath = FILpathAppend(Dir, Path);
      FILaddPathSeparator(FullPath);
      bool All = !Development;
      if(!GITcommits(FullPath, &Content, &Error, true, All)){
         continue;
      };
      COLvar Output;
      GITparseCommits(Content, &Output);
      for (int i = 0; i < Output.size(); i++){
         COLvar Log = Output[i];
         Log["path"] = Path;
         COLstring Timestamp = Log["timestamp"].asString();
         COLstring Hash = Log["hash"].asString();
         COLstring Key = Timestamp + Hash;
         if(Result.exists(Key)) {
            COL_TRC("Duplicate commit found: " + Key);
            continue;
         }
         Result[Key] = Log;
      }
   }
   COL_VAR(Result.json(1));
   return true;
}

