#ifndef __CMPC_CURRENT_COMMIT_TRANSLATOR_H__
#define __CMPC_CURRENT_COMMIT_TRANSLATOR_H__
struct CFGmap;
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCcurrentCommitTranslator
//
// Description:
//
// This is for the translator commit info - the information differs depending on if we are in the run or the dev mode.
// with an extra parameter.
//
// Author: Eliot Muir 
// Date:   Monday 27 March 2023 - 02:55PM
// ---------------------------------------------------------------------------
class COLwebRequest;
class SCKloop;

struct CFGmap;

void CMPCcurrentCommitTranslator(const COLwebRequest& Request, SCKloop* pLoop);

#endif // end of defensive include
