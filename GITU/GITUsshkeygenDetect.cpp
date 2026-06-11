// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUsshkeygenDetect
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 08 March 2023 - 02:25PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUsshkeygenDetect.h>
#include <PRO/PROwhich.h>
COL_LOG_MODULE;

// This function really will only work for windows
static COLstring GITUsshProgramFind(const COLstring& Program){
   COL_FUNCTION(GITUsshProgramFind);
   COLstring Result;
   if (PROwhich(&Result, "git")){
      COL_VAR(Result);
      Result = FILpathDir(Result);
      COL_VAR(Result);
      Result = Result.substr(0, Result.size()-4);
      COL_VAR(Result);
      Result += "usr\\bin\\" + Program + ".exe";
      if (FILfileExists(Result)){
         return Result;
      }
   }
   return "";
}

static COLstring GITUsshSmartFind(const COLstring& Program){
   COL_FUNCTION(GITUsshSmartFind);
   COLstring Key;
   bool Found = PROwhich(&Key, Program);
   if (!Found){
      Key = GITUsshProgramFind(Program);
   }
   COL_VAR(Key);
   return Key;
}

COLstring GITUsshkeygenDetect(){
   COL_FUNCTION(GITUsshkeygenDetect);
   return GITUsshSmartFind("ssh-keygen");
}

COLstring GITUsshDetect(){
   COL_FUNCTION(GITUsshDetect);
   return GITUsshSmartFind("ssh");
}