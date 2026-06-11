//---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testLicense
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Thursday, January 18th, 2024 @ 12:40:40 PM
//---------------------------------------------------------------------------
#include "testLicense.h"
#include <UNIT/UNITapp.h>

#include <DRX/DRXinformation.h>
#include <DRX/DRXserialization.h>
#include <DRX/DRXutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#define IGUANA_ID "IPCIBLBDMA" // All of the "answer" licenses that we test against are generated using this as the Iguana Id

static DRXinformation License50Components(){
   COL_FUNCTION(License50Components);
   return DRXinformation(0, 50, 0, 0);
}

static DRXinformation License150Components(){
   COL_FUNCTION(License150Components);
   return DRXinformation(0, 150, 0, 0);
}

void testEncrypt(){
   COL_FUNCTION(testEncrypt);
   DRXinformation RegistrationInformation = License50Components();
   COLstring Answer = "BA79CACE6A93023A6FC6F9AD4D60426A448B9D8A3CCC98503D83F6E675CBD134C4CA4238A0B923820DCC509A6F75849B4D4222BB0218762E8282C200A39FF7FE6A067953A084A8DEE4B290B8CBDD93A6";
   COLstring SeralizedLicense;
   DRXserializeLicense(RegistrationInformation, IGUANA_ID, &SeralizedLicense);
   UNIT_ASSERT_EQUALS(Answer, SeralizedLicense);
}

void testDecrypt(){
   COL_FUNCTION(testDecrypt);
   DRXinformation RegistrationInformation;   
   // Use the serialized license from testEncrypt
   COLstring Answer = "BA79CACE6A93023A6FC6F9AD4D60426A448B9D8A3CCC98503D83F6E675CBD134C4CA4238A0B923820DCC509A6F75849B4D4222BB0218762E8282C200A39FF7FE6A067953A084A8DEE4B290B8CBDD93A6";
   DRXparseLicense(Answer, IGUANA_ID, &RegistrationInformation);
   
   UNIT_ASSERT_EQUALS(0,     RegistrationInformation.expiryTimestamp());
   UNIT_ASSERT_EQUALS(50,    RegistrationInformation.componentLimit());
   UNIT_ASSERT_EQUALS(0,     RegistrationInformation.logSearchLimit());
   UNIT_ASSERT_EQUALS(0,     RegistrationInformation.messageRateLimit());
}

void testComponentLimit(){
   COL_FUNCTION(testComponentLimit);
   DRXinformation Own150 = License150Components();
   DRXinformation Own50  = License50Components();
   UNIT_ASSERT_EQUALS(150, Own150.componentLimit());
   UNIT_ASSERT_EQUALS(50,  Own50.componentLimit());
}

void testLicense(UNITapp* pApp){
   COL_FUNCTION(testLicense);
   pApp->add("license/encrypt",         &testEncrypt);
   pApp->add("license/decrypt",         &testDecrypt);
   pApp->add("license/limit/component", &testComponentLimit);
}