// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INIcompare
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 26 March 2023 - 11:47AM
// ---------------------------------------------------------------------------

#include <INI/INIcompare.h>
#include <INI/INIdata.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void INIsortFile(const COLstring& File){
   COL_FUNCTION(INIsortFile);
   INIdata Data;
   COLstring Content;
   FILreadFile(File, &Content);
   INIparse(Content, &Data);
   Content.clear();
   INIformat(Data, &Content);
   FILwriteFile(File, Content);
}
