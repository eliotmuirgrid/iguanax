// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CANdir
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 15 January 2025 - 12:30PM
// ---------------------------------------------------------------------------

#include <CAN/CANdir.h>

#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>

#include <PRO/PROexecute.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool CANisCpp(const COLstring& FileName){
   COL_FUNCTION(CANisCpp);
   COLstring Extn = FILpathExtLast(FileName, false);
   COL_VAR(Extn);
   return "cpp" == Extn;  // we ignore headers and makefiles - just cpp files.
}

void CANcreated(const COLstring& Dir, const COLstring& File){
   COL_FUNCTION(CANcreated);
   COLcout << Dir << File << newline;
   COLstring Out, Err;
   int Result = PROexecuteString(Dir, "git log --reverse --format=\"%aN %aD\" -- " + File, &Out, &Err);
   COLcout << Out << newline;
}

void CANdir(const COLstring& Dir){
   COL_FUNCTION(CANdir);
   FILdirEnumerator Enum(Dir + FIL_ANYFILE, false);
   COLstring File;
   while (Enum.getFile(File)){
      if (CANisCpp(File)){
         CANcreated(Dir, File);
      }   
   }
}
