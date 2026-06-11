#ifndef __TRN_TARGET_SET_H__
#define __TRN_TARGET_SET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNtargetSet
//
// Description:
//
// Routine to target what we need for annotations.  In it's own cpp for tracing.
//
// Author: Eliot Muir 
// Date:   Tuesday 06 June 2023 - 03:50PM
// ---------------------------------------------------------------------------

#include <COL/COLhashmap.h>

class TRNfile;
class COLstring;
class COLvar;
class TRNfunction;
class TRNannotationState;

void TRNtargetSet(COLhashmap<COLstring, TRNfile*>& LuaFileList, const COLstring& ProjectPath, const COLstring& SourceFile, int StartLine, const COLvar& Params);

void TRNfunctionBlock(const COLvar& P, TRNannotationState* pTranslator, const COLstring& FileName);

#endif // end of defensive include
