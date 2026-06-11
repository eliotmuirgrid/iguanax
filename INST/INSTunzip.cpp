// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTunzip
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 29 August 2023 - 11:06AM
// ---------------------------------------------------------------------------
#include <INST/INSTunzip.h>

#include <FIL/FILutils.h>
#include <MZIP/MZIPunzip.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool INSTunzip(const COLstring& From, const COLstring& To){
   COL_FUNCTION(INSTunzip);
   COL_VAR2(From, To);
   COLstring Blah;
   COLostream NullStream(Blah);
   COLarray<COLstring> ExtractedFiles;
   FILmakeFullDir(To, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   MZIPunzipUnpackZipFile(From, To, true, NULL, &NullStream, &ExtractedFiles);
   COL_VAR(ExtractedFiles.size());
   return ExtractedFiles.size() != 0;
}