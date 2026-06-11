#ifndef __SDBU_UTILS_H__
#define __SDBU_UTILS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBUutils
//
// Description:
//
// Common utility functions for use in the SDB apis.
//
// Author: Eliot Muir
// Date:   Thursday 11 May 2023 - 01:14PM
// ---------------------------------------------------------------------------
#include <ctime>
#include <SDB/SDBmessage.h>

class COLstring;
class SDBmessage;
struct CFGmap;

bool SDBUcomponentId(const COLstring& Input, COLstring* pResolvedId, COLstring* pErr);

bool SDBUcomponentExists(const CFGmap* pMap, const COLstring& Component);

void SDBUcontentToVar(const SDBmessage& Message, COLvar* pOut);

// caller's responsibility to free the returned message.
SDBmessage* SDBUextractMessage(const COLstring& Component, int Index, COLuint64 Time, COLstring* pErr,
                               const COLstring& LogDir = SDBroot(), const COLstring& LogDir2 = SDBroot2());

bool SDBUextractMessage(const COLstring& Component, int Index, COLuint64 Time, COLstring* pData, COLstring* pErr);

bool SDBUbadBounds(const SDBmessageId& Start, const SDBmessageId& End, bool Forward);

#endif // end of defensive include
