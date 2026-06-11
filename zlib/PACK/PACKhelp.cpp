//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PACKhelp.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   23/06/25 2:58 PM
//  ---------------------------------------------------------------------------
#include <PACK/PACKhelp.h>
#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <COL/COLvar.h>
COL_LOG_MODULE;

const COLstring PACK_HELP_HASH = "7ab4821d5304ef493d0218e67086f202/"; // IX-3296

COLstring PACKreplaceHelpPrefix(const COLstring& RootDir, const COLstring& File) {
   COL_FUNCTION(PACKreplaceHelpPrefix);
   COLstring NewFileName = File;
   COLstring Prefix;
#ifdef _WIN32
   Prefix = "..\\Iguana\\app\\help\\";
#else
   Prefix = "/help/";
#endif
   FILcorrectPathSeparators(Prefix);
   const size_t RootFound = NewFileName.find(RootDir);
   if(RootFound != npos) { NewFileName = NewFileName.substr(RootFound + RootDir.size() - 1); }
   if(NewFileName.find(Prefix) == 0) {
      NewFileName = PACK_HELP_HASH + NewFileName.substr(Prefix.size());
      COL_TRC("Changed filename to " << NewFileName);
   }
   return NewFileName;
}

void PACKaddHelpHash(COLmap<COLstring, COLstring>* pFileMap) {
   COL_FUNCTION(PACKaddHelpHash);
   pFileMap->add("help_hash", PACK_HELP_HASH);
}