// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGEencrypt
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 29 October 2024 - 09:59AM
// ---------------------------------------------------------------------------

#include <LOGE/LOGEencrypt.h>

#include <DIR/DIRpath.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


// /log/encryption
// Make sure not to modify pUseEncryption in this api
void LOGEencrypt(const COLwebRequest& Request, bool* pUseEncryption, SCKloop* pLoop){
   COL_FUNCTION(LOGEencrypt);
   COLvar Result;
   COL_TRC("Encryption enabled? " << *pUseEncryption);
   Result["encyption"] = *pUseEncryption;
   return COLrespondSuccess(Request.Address, Result);
}
