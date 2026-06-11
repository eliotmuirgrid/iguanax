// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTUNaes
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Friday 16 June 2023 - 11:09AM
// ---------------------------------------------------------------------------
#include "testTUNaes.h"

#include <UNIT/UNITapp.h>

#include <TUN/TUNaes.h>

#include <SFI/SFIaesEncryptor.h>
#include <SFI/SFIaesDecryptor.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testAes(const SFIaesEncryptor& AesE,const SFIaesDecryptor& AesD, const COLstring& In){
   COL_FUNCTION(testAes);
   COLstring Data = In;
   TUNaesEncrypt(AesE, &Data);
   TUNaesDecrypt(AesD, &Data);
   UNIT_ASSERT_EQUALS(In, Data);
}

static void testAesDescryption(){
   COL_FUNCTION(testAesDescryption);
   SFIaesEncryptor AesE((const unsigned char*)"1234567890123456", 16);
   SFIaesDecryptor AesD((const unsigned char*)"1234567890123456", 16);
   testAes(AesE, AesD, "The quick brown fox jumped over the lazy dog.");
   testAes(AesE, AesD, "The quick brown fox jumped over the lazy iguana.");
   
   
}

void testTUNaes(UNITapp* pApp) {
   pApp->add("aes/simple", &testAesDescryption);
}
