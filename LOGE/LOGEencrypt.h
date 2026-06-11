#ifndef __LOGE_ENCRYPT_H__
#define __LOGE_ENCRYPT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGEencrypt
//
// Description:
//
// Api to check whether log encryption is on or off
//
// Author: Vismay Shah 
// Date:   Tuesday 29 October 2024 - 09:59AM
// ---------------------------------------------------------------------------
#include <COL/COLweb.h>
class SCKloop;

// this is the web api handler
void LOGEencrypt(const COLwebRequest& Request, bool* pUseEncryption, SCKloop* pLoop);

#endif // end of defensive include
