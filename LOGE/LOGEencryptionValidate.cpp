// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGEencryptionValidate
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
 
// Date:   Friday 01 November 2024 - 02:09PM
// ---------------------------------------------------------------------------
#include <LOGE/LOGEencryptionValidate.h>

#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <SDB/SDBcrypto.h>
#include <SDB/SDBdir.h>
#include <SFI/SFIbase64DecodeFilter.h>
#include <SFI/SFIbase64EncodeFilter.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool LOGEcheckEncryptionKeySize(const COLstring& Key){
   COL_FUNCTION(LOGEcheckEncryptionKeySize);
   return Key.size() == 32;
}

bool LOGEwriteEncryptionValidation(const COLstring& EncKey, COLvar* pLogConfig){
   COL_FUNCTION(LOGEwriteEncryptionValidation);
   COLvar& LogConfig = *pLogConfig;
   COLstring RawIv, RawOut, RawTag;
   if (!SDBencrypt(LOGE_ENCRYPT_VALIDATION, EncKey, RawIv, &RawOut, &RawTag)){ return false; }
   COLstring Iv, Tag, Checksum;
   SFIbase64EncodeOneLine(RawIv,  &Iv); // base 64 encoding to prevent .json from messing up the data during serialization
   SFIbase64EncodeOneLine(RawTag, &Tag);
   SFIbase64EncodeOneLine(RawOut, &Checksum);
   LogConfig["iv"]       = Iv;
   LogConfig["tag"]      = Tag;
   LogConfig["checksum"] = Checksum;
   return true;
}

bool LOGEstartupEncryptionVerifyKey(const COLstring& Key, COLstring* pError){
   COL_FUNCTION(LOGEstartupEncryptionVerifyKey);
   COLvar LogConfig;
   COLstring Error;
   if (!SDBreadLogConfig(&LogConfig, &Error)){
      COL_TRC("Failed to read log config file");
      *pError = "Could not read log config file.";
      return false;
   }
   if (!(LogConfig.exists("iv") && LogConfig.exists("tag") && LogConfig.exists("checksum"))) { 
      *pError = "Missing required information to validate encryption key"; 
      return false; 
   }
   COLstring PlainText, RawIv, RawTag, RawChecksum;
   SFIbase64Decode(LogConfig["iv"],       &RawIv);
   SFIbase64Decode(LogConfig["tag"],      &RawTag);
   SFIbase64Decode(LogConfig["checksum"], &RawChecksum);
   bool DecryptionSuccess = SDBdecrypt(RawChecksum, RawTag, RawIv, Key, &PlainText);
   if (!DecryptionSuccess) { 
      *pError = "Failed to decrypt data";
      return false;
   }
   bool KeyOk = (PlainText == LOGE_ENCRYPT_VALIDATION);
   if (!KeyOk){
      *pError = "Incorrect encryption key";
      return false;
   }
   return true;
}
