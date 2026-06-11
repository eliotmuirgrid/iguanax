// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLsimulateOs
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 14 August 2023 - 08:51AM
// ---------------------------------------------------------------------------

#include <COL/COLsimulateOs.h>

#include <stdlib.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring s_COLos;

COLstring COLsimulatedOs(){
   COL_FUNCTION(COLsimulatedOs);
   return s_COLos;
}

void COLsimulateOS(){
   COL_FUNCTION(COLsimulateOS);
#ifdef _WIN32
   s_COLos = "windows";
#else
   #ifdef __APPLE__
      s_COLos = "macos";
   #else
      s_COLos = "linux";
   #endif
#endif
   if (getenv("COL_OS")){
      COLstring NewOs = getenv("COL_OS");
      if (NewOs != "macos" && NewOs != "linux" && NewOs != "windows"){
         COLcout << "COL_OS must be windows, macos or linux.  Ignoring COL_OS = " << NewOs << newline;   
      } else {
         s_COLos = NewOs;
         COLcout << "Warning running as simulated OS using environmental variable COL_OS = " << s_COLos << newline;
      }
   }
}

bool COLisWindows(){
   COL_FUNCTION(COLisWindows);
   bool Result = s_COLos == "windows";
   COL_VAR(Result);
   return Result;
}