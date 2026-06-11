// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTUNpkcs7Pad
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Friday 16 June 2023 - 08:38AM
// ---------------------------------------------------------------------------
#include "testTUNpkcs7Pad.h"

#include <UNIT/UNITapp.h>
#include <TUN/TUNpkcs7Pad.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testPad(const COLstring& Data){
   COLstring In = Data;
   TUNpkcs7Pad(&In);
   UNIT_ASSERT_EQUALS(0, In.size() % 16);
   TUNpkcs7Unpad(&In);
   UNIT_ASSERT_EQUALS(Data, In);
}

static void testPadding(){
   COL_FUNCTION(testPadding);
   testPad("This quick brown fox jumped over the lazy dog.");
   testPad("Diamonds are a problem is some places in the net");
   testPad("");
}

void testTUNpkcs7Pad(UNITapp* pApp) {
   pApp->add("pkcs/pad", &testPadding);
   
}
