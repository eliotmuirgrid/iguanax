// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGEgenKey
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 14 November 2024 - 04:22PM
// ---------------------------------------------------------------------------
#include <LOGE/LOGEgenKey.h>
#include <openssl/rand.h>

#include <SDB/SDBcrypto.h>
#include <SFI/SFIbase64EncodeFilter.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void LOGEgenKey(const COLwebRequest& Request){
   COL_FUNCTION(LOGEgenKey);
   COLvar Result;
   COLstring Key(SDB_AES_GCM_SIZE_KEY, '\0');
   COLstring EncodedKey;
   RAND_bytes((unsigned char*) Key.data(), Key.size());
   SFIbase64Encode(Key, &EncodedKey);
   EncodedKey = EncodedKey.substr(0, 32);
   COL_VAR2(EncodedKey, EncodedKey.size());
   Result["key"] = EncodedKey;
   COLrespondSuccess(Request.Address, Result);
}