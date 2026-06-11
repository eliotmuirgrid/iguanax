//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdriverFind
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wed 14 Apr 2021 16:27:25 EDT
//---------------------------------------------------------------------------
#include "DBdriverFind.h"
#include "DBdll.h"

#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <COL/COLlist.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// JULIAN we might want to change the LogStream stuff to tracing
//        but this will be for startup only so maybe it is ok
//        - perhaps we might want to add tracing as well???

// JULIAN recursive search -- forbid program files directory as it is really BIG

void DBsearchSubDir(const COLstring& Base, const COLstring& Search, const COLstring& Sub, COLarray<COLstring>* pList, COLostream& Stream){
   COL_FUNCTION(DBsearchSubDir); 
   COL_VAR2(Base, Sub);
   FILdirEnumerator Enum(Base + Search);
   COLstring FileName;
   while (Enum.getFile(FileName)){
       Stream << "Adding to search " << Base << FileName << Sub << newline;
       pList->push_back(Base + FileName + Sub);
   };
}

// Returns true if we can find the program files directory for windows and set's it in the pDir output.  \ will be appended.
bool DBprogramFilesDir(COLstring* pDir){
   COL_FUNCTION(DBprogramFilesDir);
   const char* pProgramFiles = getenv("programfiles");
   if (pProgramFiles){
      *pDir = pProgramFiles;
      FILaddPathSeparator(*pDir);
      return true;
   }
   return false;   
}
// JULIAN TODO Potential interface for list of results (maybe as different function)
// int DBfindMySqlCandidates(COLstring* pDriverPreferredPathResult, COLarray<COLstring>* pList, COLostream& LogStream){
bool DBfindMySql(COLstring* pDriverPathResult, COLostream& LogStream){
   COL_FUNCTION(DBfindMySql);
   #ifdef _WIN32
      COLarray<COLstring> List;
      COLstring ProgramFiles;
      if (DBprogramFilesDir(&ProgramFiles)){
         LogStream << "Found program files in " << ProgramFiles << " so searching under " << ProgramFiles << "MySQL\\" << newline;
         ProgramFiles += "MySQL\\";
         DBsearchSubDir(ProgramFiles, "MySQL*", "\\bin\\", &List, LogStream);  // seems like the library used to be "bin"
         DBsearchSubDir(ProgramFiles, "MySQL*", "\\lib\\", &List, LogStream);  // but now may be in "lib" so we should search both...
      }
      if (DBdllFindInDirList(List, "libmySQL.dll", pDriverPathResult, &LogStream)){
         return true;
      }
      COLarray<COLstring> PathList;  
      DBsearchPaths("PATH", &PathList, &LogStream);
      return DBdllFindInDirList(PathList, "libmySQL.dll", pDriverPathResult, &LogStream);
   #elif defined(__APPLE__)
      COLarray<COLstring> List;
      DBsearchPaths("DYLD_LIBRARY_PATH", &List, &LogStream);
      // List.push_back("/usr/local/lib"); // JULIAN the link in here connected to MariaDB DLL and segfaulted
      List.push_back("/usr/local/mysql/lib"); 
      return DBdllFindInDirList(List, "libmysqlclient*dylib", pDriverPathResult, &LogStream);
   #else // Linux
      COLarray<COLstring> List;
      DBsearchPaths("LD_LIBRARY_PATH", &List, &LogStream);
      List.push_back("/usr/lib64/mysql/");
      return DBdllFindInDirList(List, "libmysqlclient.so*", pDriverPathResult, &LogStream);
   #endif
   return false;
}

bool DBfindMariaDb(COLstring* pDriverPathResult, COLostream& LogStream){
   COL_FUNCTION(DBfindMariaDb);
   #ifdef _WIN32
      COLarray<COLstring> List;
      COLstring ProgramFiles;
      if (DBprogramFilesDir(&ProgramFiles)){
         LogStream << "Found program files in " << ProgramFiles << " so searching for MariaDB instances." << newline;
         DBsearchSubDir(ProgramFiles, "MariaDB*", "\\lib\\", &List, LogStream);
      }
      if (DBdllFindInDirList(List, "libmariadb.dll", pDriverPathResult, &LogStream)){
         return true;
      }
      COLarray<COLstring> PathList;  
      DBsearchPaths("PATH", &PathList, &LogStream);
      return DBdllFindInDirList(PathList, "libmariadb.dll", pDriverPathResult, &LogStream);
   #elif defined(__APPLE__)
      COLarray<COLstring> List;
      DBsearchPaths("DYLD_LIBRARY_PATH", &List, &LogStream);
      List.push_back("/usr/local/lib");
      return DBdllFindInDirList(List, "libmariadb*dylib", pDriverPathResult, &LogStream);
   #else // Linux
      COLarray<COLstring> List;
      DBsearchPaths("LD_LIBRARY_PATH", &List, &LogStream);
      List.push_back("/usr/lib64/");
      List.push_back("/usr/lib64/mariadb/");
      return DBdllFindInDirList(List, "libmariadb.so*", pDriverPathResult, &LogStream);
   #endif
   return false;
}

bool DBfindOdbc(COLstring* pDriverPathResult, COLostream& LogStream){
   COL_FUNCTION(DBfindOdbc);
   #ifdef _WIN32
      // For Windows, we know where the file is located. No need to do an enumeration to figure it out, so we just call
      // FILfileExists directly.
      const COLstring& WindowDir = FILwindowsDir();
      LogStream << "Checking for odbc dll Windows System directory " << WindowDir << "SYSTEM32\\" << newline;
      COLstring OdbcDefaultPath = FILpathAppend(FILpathAppend(WindowDir, "SYSTEM32"), "odbc32.dll");
      COL_VAR2(OdbcDefaultPath, FILfileExists(OdbcDefaultPath));
      if(FILfileExists(OdbcDefaultPath)) {
         pDriverPathResult->swap(&OdbcDefaultPath);
         return true;
      }
   #elif defined(__APPLE__)
      COLarray<COLstring> List;
      DBsearchPaths("DYLD_LIBRARY_PATH", &List, &LogStream);
      COLstring UnixOdbcDir;
      if (DBdllFindLatestBrewOdbc(&UnixOdbcDir, &LogStream)){
         List.push_back(UnixOdbcDir);
      }
      List.push_back("/usr/lib/");
      return DBdllFindInDirList(List, "libodbc*dylib", pDriverPathResult, &LogStream) ||
             DBdllFindInDirList(List, "libiodbc*dylib", pDriverPathResult, &LogStream);
   #else // Linux
      COLarray<COLstring> List;
      DBsearchPaths("LD_LIBRARY_PATH", &List, &LogStream);
      List.push_back("/usr/lib64/");
      return DBdllFindInDirList(List, "libodbc.so*", pDriverPathResult, &LogStream);
   #endif
   return false;
}

// best info was from the three (Win/Linux/Mac) Oracle instanclient download pages 
bool DBfindOracleOci(COLstring* pDriverPathResult, COLostream& LogStream){
   COL_FUNCTION(DBfindOracleOci);
   #ifdef _WIN32
      COLarray<COLstring> List;
      LogStream << "Checking for Oracle OCI (oci.dll) in Windows PATH and TNS_ADMIN system variables" << newline;
      DBsearchPaths("PATH", &List, &LogStream);
      DBsearchPaths("TNS_ADMIN", &List, &LogStream);
      DBsearchSubDir("C:\\oracle\\", "instantclient*", "", &List, LogStream);
      return DBdllFindInDirList(List, "oci.dll", pDriverPathResult, &LogStream);
   #elif defined(__APPLE__)
     COLarray<COLstring> List;
      DBsearchPaths("DYLD_LIBRARY_PATH", &List, &LogStream);
      DBsearchPaths("TNS_ADMIN", &List, &LogStream);
      DBsearchSubDir("~", "instantclient*", "", &List, LogStream);
      List.push_back("/usr/local/lib/");
      List.push_back("~/lib/");
     return DBdllFindInDirList(List, "libclntsh*dylib", pDriverPathResult, &LogStream);
   #else // Linux
      COLarray<COLstring> List;
      DBsearchPaths("LD_LIBRARY_PATH", &List, &LogStream);
      DBsearchPaths("TNS_ADMIN", &List, &LogStream);
      DBsearchSubDir("/opt/oracle/", "instantclient*", "", &List, LogStream);
      DBsearchSubDir("/usr/lib/oracle/", "instantclient*", "", &List, LogStream);
      DBsearchSubDir("/usr/include/oracle/", "instantclient*", "", &List, LogStream);
      List.push_back("/usr/lib/");
      return DBdllFindInDirList(List, "libclntsh.so*", pDriverPathResult, &LogStream);
   #endif
   return false;
}

