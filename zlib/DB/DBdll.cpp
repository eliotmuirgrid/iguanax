//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdll
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 13 Jul 2020 15:44:10 EDT
//
//---------------------------------------------------------------------------

#include "DBdll.h"

#include <FIL/FILdirEnumerator.h>
#include <FIL/FILutils.h>

#include <COL/COLarray.h>
#include <COL/COLsplit.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool DBhasNoNumber(const COLstring& FileName){
   for (int i=0; i < FileName.size(); i++){
      if (FileName[i] >= '0' && FileName[i] <= '9'){
         return false;
      }
   }
   return true;
}

static int DBmajorVersion(const COLstring& FileName){
   COL_FUNCTION(DBmajorVersion);
   COLarray<COLstring> Parts;
   COLsplit(&Parts, FileName, ".");
   for (int i=0; i < Parts.size(); i++){
      if (atoi(Parts[i].c_str()) > 0){
         return atoi(Parts[i].c_str());
      }
   }
   return -1;
}

bool DBdllFind(const COLstring& iDir, const COLstring& Match, COLstring* pResult, COLostream* pOutStream){
   COL_FUNCTION(DBdllFind);
   COLostream& Out = *pOutStream;
   COLstring Dir = iDir;
   FILaddPathSeparator(Dir);

   Out << "Searching " << Dir << newline;
   FILdirEnumerator Enum(Dir + Match);
   COLstring FileName;
   int HighestVersion = 0;
   while (Enum.getFile(FileName)){
      Out << FileName;
      if (DBhasNoNumber(FileName)){
         #ifdef _WIN32
         Out << " match" << newline;   //under windows we don't have the conventions around version numbers on DLLs.
         #else
         Out << " best match since there is no version number" << newline;
         #endif
         *pResult = Dir + FileName;
         return true;
      } else {
         int Version = DBmajorVersion(FileName);
         if (Version > HighestVersion){
            HighestVersion = Version;
            *pResult = Dir + FileName;
            Out << " best match so far";
         } 
         Out << " (major version=" << DBmajorVersion(FileName) << ")";
      }
      Out << newline;
   }
   return (HighestVersion > 0);
}

bool DBsearchPaths(const COLstring& Environment, COLarray<COLstring>* pList, COLostream* pOutStream){
   COL_FUNCTION(DBsearchPaths);
   COLostream& Out = *pOutStream;
   COLarray<COLstring>& List = *pList;
   const char* pEnv = getenv(Environment.c_str());
   if (!pEnv){
      Out << "Environment shared library path variable " << Environment << " not detected." << newline;
      return false;
   }
   Out << "Searching directories from " << Environment << newline;
   COLsplit(pList, pEnv, FIL_PATH_SEPARATOR);  // note that this code will work cross platform
   for (int i=0; i < List.size(); i++){
      Out << " " << List[i] << newline;
   }
   return true;
}

bool DBdllFindInDirList(const COLarray<COLstring>& List, const COLstring& Match, COLstring* pResult, COLostream* pOutStream){
   COL_FUNCTION(DBdllFindInDirList);
   for (int i=0; i < List.size(); i++){
      if (DBdllFind(List[i], Match, pResult, pOutStream)){
         return true;
      }
   }
   return false;
}

// expect N.N.N
static int DBextractVersion(const COLstring& FileName){
   COL_FUNCTION(DBextractVersion);
   COLarray<COLstring> Numbers;
   COLsplit(&Numbers, FileName, ".");
   int Result = 0;
   for (int i=0; i < Numbers.size() ; i++){
      Result = Result * 100 + atoi(Numbers[i].c_str());
   }
   COL_VAR2(FileName, Result);
   return Result;
}

bool DBdllFindLatestBrewOdbc(COLstring* pLibDir, COLostream* pOutStream){
   COL_FUNCTION(DBdllFindLatestBrewOdbc);
   COLostream& Out = *pOutStream;
   if (!FILfileExists("/usr/local/Cellar/unixodbc/")){
      Out << "No Unix ODBC found at /usr/local/Cellar/unixobdc/" << newline;
      return false;
   }
   FILdirEnumerator Enum("/usr/local/Cellar/unixodbc/*");
   COLstring FileName;
   COLstring Match;
   int HighestVersion = 0;
   while (Enum.getFile(FileName)){
      int Version = DBextractVersion(FileName);
      if (Version > HighestVersion){
         HighestVersion = Version;
         Match = FileName;
      }
   }
   // TODO - what if we cannot find it
   *pLibDir = "/usr/local/Cellar/unixodbc/" + Match + "/lib/";
   Out << "Found unixodbc dir at " << *pLibDir << newline;
   COL_VAR3(*pLibDir, FileName, Match);
   return true;
}

