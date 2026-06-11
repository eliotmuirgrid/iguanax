//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFITcrypt3
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, January 15th, 2013 @ 03:52:42 PM
//
//---------------------------------------------------------------------------
#include "SFITprecomp.h"
#pragma hdrstop

#include "SFITcrypt3.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <SFI/SFIcrypt3.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void testCrypt3(){
   static const COLstring MyPassword = "abc123def456";
   static const COLstring MySalt = "hi";
   static const COLuint32 CountOfIteration = 20;

   int i;
   COLstring MyHashedPassword;

   COL_TRC("Encrypting (hybrid DES) '" << MyPassword << "' with custom salt '" << MySalt << "':" << newline);
   SFIcrypt3::crypt(MyPassword, &MySalt, MyHashedPassword);
   COL_TRC("Encrypted password: [" << MyHashedPassword.substr(0, 2) << "]" <<
      MyHashedPassword.substr(2, MyHashedPassword.size()) << newline << newline);

   COL_TRC("Running " << CountOfIteration << " iterations to check random salt encryption:" << newline);
   COL_TRC("First two bytes enclosed in [] of ciphertext are the salt used to encrypt." << newline << newline);

   for (i = 0; i < CountOfIteration; i++){
      SFIcrypt3::crypt(MyPassword, NULL, MyHashedPassword);
      COL_TRC("With random salt: [" << MyHashedPassword.substr(0, 2) << "]" << 
         MyHashedPassword.substr(2, MyHashedPassword.size()) << newline << newline);
   }

   COL_TRC("Encrypting (hybrid MD5) '" << MyPassword << "' with custom salt '" << MySalt << "':" << newline);
   SFIcrypt3::md5crypt(MyPassword, &MySalt, MyHashedPassword);
   COL_TRC("Encrypted password: " << MyHashedPassword << newline << newline);

   COL_TRC("Running " << CountOfIteration << " iterations to check random salt encryption:" << newline);
   COL_TRC("String between second and third '$' characters of ciphertext are the salt used to encrypt." << newline << newline);   

   for (i = 0; i < CountOfIteration; i++){
      SFIcrypt3::md5crypt(MyPassword, NULL, MyHashedPassword);
      COL_TRC("Generated password: " << MyHashedPassword << newline << newline);
   }
}

TSTtestable* SFITcrypt3()
{
   TST_START_COLLECTION("SFITcrypt3")
      TST_ADD_TEST_FUNCTION(testCrypt3);
   TST_END_FIXTURE
};

