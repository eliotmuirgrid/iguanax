// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBsize
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 11 May 2023 - 01:26PM
// ---------------------------------------------------------------------------

#include <SDB/SDBsize.h>

#include <FIL/FILdirEnumerator.h>
#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLint64 SDBsize(const COLstring& Dir){
   COL_FUNCTION(SDBsize);
   if (!FILfileExists(Dir)){
      COL_TRC("File does not exist so returning zero.");
      return 0;
   }
   COL_VAR(Dir);
   COLint64 Size = 0;
   COLstring Root = Dir;
   FILaddPathSeparator(Root);
   FILdirEnumerator Enum(Root + FIL_ANYFILE);
   COLstring FileName;
   while (Enum.getFile(FileName)){
      if (Enum.isDirectory()){
         Size += SDBsize(Root + FileName);
      } else {
         COL_VAR2(FileName, Enum.fileSize());
         Size += Enum.fileSize();
      }
   }
   COL_VAR(Size);
   return Size;
}