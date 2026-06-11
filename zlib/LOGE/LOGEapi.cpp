// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGEapi
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 29 October 2024 - 09:42AM
// ---------------------------------------------------------------------------

#include <LOGE/LOGEapi.h>
#include <LOGE/LOGEencrypt.h>
#include <LOGE/LOGEencryptionSet.h>
#include <LOGE/LOGEencryptionPermission.h>
#include <LOGE/LOGEgenKey.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void LOGEapiMain(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, bool* pUseEncryption, 
                 BLUAinstanceManager* pManager, SCKloop* pLoop){
   COL_FUNCTION(LOGEapiMain);
   ApiMap.add("/log/encryption",             COLnewClosure1(&LOGEencrypt,    pUseEncryption, pLoop));
   ApiMap.add("/log/encryption/set",         COLnewClosure1(&LOGEencryptSet, pUseEncryption, pManager, pLoop));
   ApiMap.add("/log/encryption/permission",  COLnewClosure1(&LOGEencryptionPermission));
   ApiMap.add("/log/encryption/gen_key",     COLnewClosure1(&LOGEgenKey));
}
