#ifndef __LNET_HTTP_DEBUG_H__
#define __LNET_HTTP_DEBUG_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNEThttpDebug
//
// Description:
//
// Functions dealing with the curl debug log
//
// Author: Vismay Shah 
// Date:   Thursday 29 June 2023 - 10:49AM
// ---------------------------------------------------------------------------
#include <COL/COLstring.h>
class COLerror;
class LNEThttpCurlItem;

/* These wrap around the CURLdebugLog defined in CURLutils */
void LNEThttpDebugStartTracing          (LNEThttpCurlItem* pItem);
void LNEThttpDebugAppendError           (LNEThttpCurlItem* pItem, const COLerror& Error);
bool LNEThttpDebugIsTracing             (const LNEThttpCurlItem* pItem);
const COLstring& LNEThttpDebugReadOutput(const LNEThttpCurlItem* pItem);

#endif // end of defensive include
