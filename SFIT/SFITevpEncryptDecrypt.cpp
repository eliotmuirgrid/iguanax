//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFITbase32
//
// Description:
//
// Implementation
//
// Author: Steven Dodd
// Date:   Friday, August 20th, 2010 @ 09:32:22 AM
//
//---------------------------------------------------------------------------
#include "SFITevpEncryptDecrypt.h"
#include <SFI/SFIevpEncryptDecrypt.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <SFI/SFIguid.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testEncryptDecrypt() {
   COL_FUNCTION(testEncryptDecrypt);
   COLstring Key = SFIgenerateGUID(256);
   COLstring IV = SFIgenerateGUID(128);
   COLstring Text = "hello!this is a test~~";
   COLstring CipherText;
   SFIevpEncrypt(Text, Key, IV, &CipherText);
   COL_VAR(CipherText);
   
   COLstring Result;
   SFIevpDecrypt(CipherText, Key, IV, &Result);
   COL_VAR(Result);
   TST_ASSERT(Result == Text);
}


TSTtestable* SFITevpEncryptDecrypt()
{
   TST_START_COLLECTION("SFITevpEncryptDecrypt")
      TST_ADD_TEST_FUNCTION(testEncryptDecrypt);
   TST_END_FIXTURE
};

