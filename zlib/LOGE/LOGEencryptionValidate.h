#ifndef __LOGE_ENCRYPTION_VALIDATE_H__
#define __LOGE_ENCRYPTION_VALIDATE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGEencryptionValidate
//
// Description:
//
// TODO
//
// Author: Vismay Shah
 
// Date:   Friday 01 November 2024 - 02:09PM
// ---------------------------------------------------------------------------
#define LOGE_ENCRYPT_VALIDATION "IguanaXIsAwesome!"
class COLstring;
class COLvar;

// used by the log encryption startup mode
bool LOGEcheckEncryptionKeySize    (const COLstring& EncKey);
bool LOGEwriteEncryptionValidation (const COLstring& EncKey, COLvar* pLogConfig);
bool LOGEstartupEncryptionVerifyKey(const COLstring& EncKey, COLstring* pError);
#endif // end of defensive include
