#ifndef __TRN_IDE_INTELLISENSE_H__
#define __TRN_IDE_INTELLISENSE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideIntellisense
//
// Description:
//
// Beginning of a new implementation of the intellisense function.
//
// Author: Eliot Muir 
// Date:   Tuesday 25 April 2023 - 01:23PM
// ---------------------------------------------------------------------------
class COLwebRequest;
class SCKloop;
struct CFGmap;

void TRNideIntellisense(const COLwebRequest& Request, SCKloop* pLoop);

#endif // end of defensive include
