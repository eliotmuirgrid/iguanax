// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROwhich
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 01 February 2023 - 10:45AM
// ---------------------------------------------------------------------------

#include <PRO/PROwhich.h>
#include <PRO/PROexecute.h>

#include <stdlib.h>

#include <FIL/FILutils.h>

#include <COL/COLsplit.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool PROwhichFromPath(const COLstring& Path, const COLstring& Program, COLstring* pResult){
   COL_FUNCTION(PROwhichFromPath);
   COL_VAR2(Program, Path);
   COLarray<COLstring> Dirs;
   COLsplit(&Dirs, Path, FIL_PATH_SEPARATOR);
   for (int i=0; i < Dirs.size(); i++){
      COLstring Expected = Dirs[i];
      Expected = Expected.strip(COLstring::Both,' ');
      Expected = Expected.strip(COLstring::Both,'\t');
      FILaddPathSeparator(Expected);
      Expected += Program;
      #ifdef _WIN32
         Expected += ".exe";
      #endif
      COL_TRC(Expected << " checking...");
      if (FILfileExists(Expected)){
         COL_TRC("Found it!");
         *pResult = Expected;
         return true;
      }
   }
   COL_TRC("Could not find " << Program);
   return false;
}


bool PROwhich(COLstring* pPath, const COLstring& Program){
   COL_FUNCTION(PROwhich);
   if (!getenv("PATH")){
      COL_ERR("No PATH variable set");
      return false;
   }
   return PROwhichFromPath(getenv("PATH"), Program, pPath);
}
   /*
   COLstring Err, Command;
   #ifdef _WIN32
      Command = "where";
   #else
      Command = "which";
   #endif
   Command += " " + Program;
   COLstring Path = getenv("PATH");
   COL_VAR(Path);
   COL_VAR2(Program, Command);
   int ExitCode = PROexecuteString("", Command, pPath, &Err);
   COL_DUMP("Path:", pPath->c_str(), pPath->size());
   COL_VAR3(ExitCode, *pPath, Err);
   pPath->stripAll('\n');
   pPath->stripAll('\r'); // for windows
   COL_DUMP("Path:", pPath->c_str(), pPath->size());
   return (ExitCode == 0);
}*/