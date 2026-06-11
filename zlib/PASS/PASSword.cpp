//-----------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  PASSword 
//
// Description
//
// Implementation
//
// Author: Shuo Hou
//-----------------------------------------------------------------------------
#include "PASSword.h"

#include <SFI/SFIhexDecodeFilter.h>
#include <SFI/SFIhexEncodeFilter.h>
#include <SFI/SFIevpEncryptDecrypt.h>

#include <COL/COLvar.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring PASSkey;
COLstring PASSiv;

void PASSsetKeyIV(const COLstring& Key, const COLstring& IV) {
   COL_FUNCTION(PASSsetKeyIV);
   COL_VAR2(Key, IV);
   PASSkey = Key;
   PASSiv = IV;
}

bool PASSdecrypt(const COLstring& Cipher, COLstring* pResult) {
   COL_FUNCTION(PASSdecrypt);
   COLASSERT(!PASSkey.is_null());
   COLASSERT(!PASSiv.is_null());
   COLstring Decoded;
   if (!SFIisHex(Cipher)){  // Eliot put this here to get rid of annoying stack traces during Iguana start up.
      *pResult = "Unable to decrypt password.";
      return false;
   }
   if(!SFIhexDecode(Cipher, &Decoded)){
      *pResult = "Unable to decrypt password: " + Decoded;
      return false;
   } else {
      SFIevpDecrypt(Decoded, PASSkey, PASSiv, pResult);
      return true;
   }
}

bool PASSencrypt(const COLstring& Password, COLstring* pResult) {
   COL_FUNCTION(PASSencrypt);
   COLASSERT(!PASSkey.is_null());
   COLASSERT(!PASSiv.is_null());
   try{
      COLstring CipherText;
      SFIevpEncrypt(Password, PASSkey, PASSiv, &CipherText);
      *pResult = SFIhexEncode(CipherText);
   } catch (const COLerror& Error) {
      COL_ERR("Failed to encrypt password.");
      *pResult = "Unable to Encrypting password";  
      return false;
   }
   return true;
}

bool PASSdecryptCustom(const COLstring& Cipher, COLstring* pResult) {
   COL_FUNCTION(PASSdecryptCustom);
   COLASSERT(!PASSkey.is_null());
   COLASSERT(!PASSiv.is_null());
   COLstring Decoded;
   if(Cipher.size() > 0 && (Cipher[0] == '{' || Cipher[0] == '[')) {
      COLvar Json;
      if(Json.parse(Cipher)) {
         COL_TRC("Plain json found");
         *pResult = Cipher;
         return false;
      }
   }
   if(!SFIhexDecode(Cipher, &Decoded)){
      COL_TRC("Unable to decode cipher");
      *pResult = Cipher;
      return false;
   } else {
      SFIevpDecrypt(Decoded, PASSkey, PASSiv, pResult);
      return true;
   }
}

bool PASSencryptCustom(const COLstring& Content, COLstring* pResult) {
   COL_FUNCTION(PASSencryptCustom);
   COLASSERT(!PASSkey.is_null());
   COLASSERT(!PASSiv.is_null());
   try{
      COLstring CipherText;
      SFIevpEncrypt(Content, PASSkey, PASSiv, &CipherText);
      COL_TRC("Successfully encrypted");
      *pResult = SFIhexEncode(CipherText);
      COL_VAR(*pResult);
   } catch (const COLerror& Error) {
      COL_ERR("Failed to encrypt content.");
      *pResult = "Unable to encrypt content";
      return false;
   }
   return true;
}
