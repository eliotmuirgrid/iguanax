//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSmakeExec
//
// Description:
//
// Implementation
// 
// Author: Eliot Muir
// Date:   Sat  6 Jul 2019 10:27:21 EDT
//---------------------------------------------------------------------------

#include "INSmakeExec.h"

#include <FIL/FILutils.h>
#include <FIL/FILdirEnumerator.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void INSmakeExec(const COLstring& InDir){
   COL_FUNCTION(INSmakeExec);
   COLstring Dir = InDir;
   FILaddPathSeparator(Dir);
   COL_VAR(Dir);
   // POSIX only
   COLstring FileName;
   FILdirEnumerator Enum(Dir + "*");
   for (; Enum.getFile(FileName);){
      COL_VAR(FileName);
      #ifndef _WIN32
         FILsetPermission(Dir+FileName, FIL_USER_RWX | FIL_GROUP_RX);
      #endif
   }
}


