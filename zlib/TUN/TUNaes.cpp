// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNaes
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 16 June 2023 - 10:57AM
// ---------------------------------------------------------------------------

#include <TUN/TUNaes.h>
#include <TUN/TUNpkcs7Pad.h>

#include <SFI/SFIaesEncryptor.h>
#include <SFI/SFIaesDecryptor.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNaesEncrypt(const SFIaesEncryptor& AesEncryptor, COLstring* pData){
   COL_FUNCTION(TUNaesEncrypt);
   COL_DUMP("Raw:", pData->c_str(), pData->size());
   TUNpkcs7Pad(pData);
   COL_DUMP("Padded:", pData->c_str(), pData->size());
   COLuint8* i = (COLuint8*)pData->get_buffer();
   COLuint8* pEnd   = i + pData->size();
   do{
      AesEncryptor.ProcessBlock(i, i);
      i +=16;
      COL_TRC((void*)i);
   } while (i != pEnd);
   COL_DUMP("Encrypted:", pData->c_str(), pData->size());
}

void TUNaesDecrypt(const SFIaesDecryptor& AesDecryptor, COLstring* pData){
   COL_FUNCTION(TUNaesDecrypt);
   COL_DUMP("Raw:", pData->c_str(), pData->size());
   COLuint8* i = (COLuint8*)pData->get_buffer();
   COLuint8* pEnd   = i + pData->size();
   do{
      AesDecryptor.ProcessBlock(i, i);
      i +=16;
      COL_TRC((void*)i);
   } while (i != pEnd);
   COL_DUMP("Decrypted:", pData->c_str(), pData->size());
   TUNpkcs7Unpad(pData);
   COL_DUMP("Unpadded:", pData->c_str(), pData->size());
}