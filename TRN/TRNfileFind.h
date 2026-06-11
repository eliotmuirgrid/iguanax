#ifndef __TRN_FILE_FIND_H__
#define __TRN_FILE_FIND_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNfileFind
//
// Description:
//
// Routines to find Lua files.
//
// Author: Eliot Muir 
// Date:   Monday 01 May 2023 - 10:28AM
// ---------------------------------------------------------------------------

#include <COL/COLhashmap.h>

class TRNfile;

// Will load the file from disk if it's not already loaded. If the file does not exist on disk then it will be created.
TRNfile* TRNfileFindOrAdd  (COLhashmap<COLstring, TRNfile*>& LuaFileList, const COLstring& ProjectPath, const COLstring& Label);

// Won't load file from disk; only tells you if the file is already loaded into this program.
const TRNfile* TRNfileFind(const COLhashmap<COLstring, TRNfile*>& LuaFileList, const COLstring& Label);

bool TRNfileLoaded(COLhashmap<COLstring, TRNfile*>& LuaFileList, const COLstring& Label);

#endif // end of defensive include
