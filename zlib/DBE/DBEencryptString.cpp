//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBEencryptString
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Wednesday, April 23rd, 2008 @ 06:26:27 PM
//
//---------------------------------------------------------------------------

#include "DBEencryptString.h"

#include <SFI/SFIbase64EncodeFilter.h>
#include <SFI/SFIbase64DecodeFilter.h>
#include <SFI/SFImd5.h>

#include <SFI/SFIaesEncryptor.h>
#include <SFI/SFIaesDecryptor.h>
#include <SFI/SFIrandom.h>

#include <COL/COLerror.h>
#include <COL/COLsinkString.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

DBEencryptionKey::DBEencryptionKey() {
   ::memset(Value,0,sizeof(Value));  
}

DBEencryptionKey::~DBEencryptionKey()
{}

const char* DEFAULT_KEY = "AAA0A8A65DA156D24EE2A03277530142";
const unsigned int KEY_SIZE = 16;

class DBEaesEncryptor : public SFIaesEncryptor
{
public:
   DBEaesEncryptor(const DBEencryptionKey& Key)
      : SFIaesEncryptor(Key.Value, sizeof(Key.Value))
   {
      COL_METHOD(DBEaesEncryptor::DBEaesEncryptor);
   }
   ~DBEaesEncryptor()
   {
      COL_METHOD(DBEaesEncryptor::~DBEaesEncryptor);
   }
};

class DBEaesDecryptor : public SFIaesDecryptor
{
public:
   DBEaesDecryptor(const DBEencryptionKey& Key)
      : SFIaesDecryptor(Key.Value, sizeof(Key.Value))
   {
      COL_METHOD(DBEaesDecryptor::DBEaesDecryptor);
   }
   ~DBEaesDecryptor() {
      COL_METHOD(DBEaesDecryptor::~DBEaesDecryptor);
   }
};

static COLstring DBEaesDecryptorDecrypt(const COLstring& Input, const DBEencryptionKey& Key) {
   COLstring Output;
   DBEaesDecryptor Decryptor(Key);

   COLuint8 InputBuffer[16];
   COLuint8 OutputBuffer[16];

   COLPRECONDITION(sizeof(InputBuffer) == SFIaes::SFI_AES_BLOCK_SIZE);
   COLPRECONDITION(sizeof(OutputBuffer) == SFIaes::SFI_AES_BLOCK_SIZE);

   const COLuint8* pCurrent = (const COLuint8*)Input.get_buffer();
   const COLuint8* pEnd = (const COLuint8*)Input.get_buffer()+Input.size();
   while(pCurrent != pEnd) {
      ::memset(InputBuffer,0,sizeof(InputBuffer));
      ::memcpy(InputBuffer,pCurrent,COL_MIN(sizeof(InputBuffer),pEnd-pCurrent) );
      pCurrent += COL_MIN(sizeof(InputBuffer),pEnd-pCurrent);

      ::memset(OutputBuffer,0,sizeof(OutputBuffer));
      Decryptor.ProcessBlock(InputBuffer,OutputBuffer);

      Output.write(OutputBuffer,sizeof(OutputBuffer));
   }

   return Output;
}

static COLstring DBEaesEncryptorEncrypt(const COLstring& Input, const DBEencryptionKey& Key) {
   COLstring Output;
   DBEaesEncryptor Encryptor(Key);

   COLuint8 InputBuffer[16];
   COLuint8 OutputBuffer[16];

   COLPRECONDITION(sizeof(InputBuffer) == SFIaes::SFI_AES_BLOCK_SIZE);
   COLPRECONDITION(sizeof(OutputBuffer) == SFIaes::SFI_AES_BLOCK_SIZE);

   const char* pCurrent = Input.c_str();
   const char* pEnd = Input.c_str()+Input.size();
   while(pCurrent != pEnd) {
      ::memset(InputBuffer,0,sizeof(InputBuffer));
      ::memcpy(InputBuffer,pCurrent,COL_MIN(sizeof(InputBuffer),pEnd-pCurrent) );
      pCurrent += COL_MIN(sizeof(InputBuffer),pEnd-pCurrent);

      ::memset(OutputBuffer,0,sizeof(OutputBuffer));
      Encryptor.ProcessBlock(InputBuffer,OutputBuffer);

      Output.write(OutputBuffer,sizeof(OutputBuffer));
   }
   return Output;
}

static const COLstring& DBEencryptedPrepend() {
   //prepend this string, so that on decryption we can detect if its properly decrypted
   static const COLstring Prepend("encrypted:");
   return Prepend;
}

COLstring DBEgenerateEncryptionSalt() {
   COL_FUNCTION(DBEgenerateEncryptionSalt);
   SFIrandom Rng;
   char RandomNumber[16] = {0};
   Rng.generate(RandomNumber,sizeof(RandomNumber));

   //base64 encode
   COLstring Output;
   {
      COLsinkString Sink(Output);
      SFIbase64EncodeFilter EncodeFilter(&Sink);
      EncodeFilter.write( RandomNumber, sizeof(RandomNumber));
      EncodeFilter.onEndStream();
   }

   //since this is what we declared in header, check output size
   COL_STATIC_ASSERT(sizeof(RandomNumber) == 16);
   COLPRECONDITION(Output.size() ==24);

   COL_VAR(Output);
   return Output;
}

const COLstring& DBEencryptionKeySeed() {
   // This seed can NEVER change, it will affect serialization of passwords. 
   // It will also break ALL auto unlockers for encrypytion locker.
   static const COLstring Key("VSfsKmJgIOdNV3r6fQXroygoyrJ2xuOK");
   return Key;
};

DBEencryptionKey DBEcreateEncryptionKey(const COLstring& Salt) {
   COL_FUNCTION(DBEcreateEncryptionKey);
   //salt before seed, with MD5
   SFImd5Hasher Hasher;
   Hasher.write(Salt.c_str(), Salt.size());
   Hasher.write(DBEencryptionKeySeed().c_str(), DBEencryptionKeySeed().size());
   
   COLstring& HashValue = Hasher.hash();

   DBEencryptionKey Return;
   COLPRECONDITION(HashValue.size() == sizeof(Return.Value));
   ::memcpy(Return.Value, HashValue.c_str(), HashValue.size());

   //build output
   return Return;
}

void DBEencryptString(const COLstring& Input, const DBEencryptionKey& EncryptionKey, COLstring* pOutput) {
   COL_FUNCTION(DBEencryptString);
   COLstring PreEncrypt(DBEencryptedPrepend());
   PreEncrypt += Input;
   COLstring Encrypted = DBEaesEncryptorEncrypt(PreEncrypt, EncryptionKey);

   //encode it
   COLstring& Output = *pOutput;
   {
      COLsinkString Sink(Output);
      SFIbase64EncodeFilter EncodeFilter(&Sink);
      EncodeFilter.write(Encrypted.get_buffer(), Encrypted.size());
      EncodeFilter.onEndStream();
   }
   COL_VAR(Output);
}

void DBEencryptString(const COLstring& Input, COLstring* pOutput) {
   COL_FUNCTION(DBEencryptString);
   DBEencryptionKey Key;
   ::memcpy(Key.Value, DEFAULT_KEY, KEY_SIZE);
   DBEencryptString(Input, Key, pOutput);
}

COLstring DBEencryptString(const COLstring& Input, const DBEencryptionKey& EncryptionKey){
   COL_FUNCTION(DBEencryptString);
   COLstring Output;
   DBEencryptString(Input, EncryptionKey, &Output);
   return Output;
}

//if success, true is returned and output contains the decrypted string
//if failed, false is returned, and output contains the reason for failure
bool DBEdecryptString(const COLstring& InputData, const DBEencryptionKey& DecryptionKey, COLstring* pOutput) {
   COL_FUNCTION(DBEdecryptString);
   //hex decode it
   COLstring& Output = *pOutput;
   Output.clear();
   COLstring Decoded;
   {
      try {
         COLsinkString Sink(Decoded);
         SFIbase64DecodeFilter DecodeFilter(&Sink);
         DecodeFilter.write(InputData.c_str(),InputData.size());
         DecodeFilter.onEndStream();
      } catch(...) {
         Output = "Input was not properly encoded";
         return false;
      }
   }

   //decrypt it
   COLstring Decrypted = DBEaesDecryptorDecrypt(Decoded, DecryptionKey);

   //verify it
   bool Valid = true;
   if (Decrypted.size() >= DBEencryptedPrepend().size()) {
      COLuint32 CharIndex = 0;
      for (CharIndex; Valid && CharIndex < DBEencryptedPrepend().size(); CharIndex++) {
         if (Decrypted.data()[CharIndex] != (COLuint8)DBEencryptedPrepend()[CharIndex]) {
            Valid = false;
         }
      }
      if (Valid) {
         COLPRECONDITION(Decrypted.size() >= CharIndex);
         const char* pDecrypted = (const char*)Decrypted.data() + CharIndex;
         COLuint32 DecryptedSize = Decrypted.size() - CharIndex;
         //we don't assume that we know the encryption block size, 
         //so we look for nulls from just
         //after 'encrypted' instead of in the last block. 
         //This is to ensure that the string
         //doesn't get null chars which tends to mess it up.
         if (const char* pNullChar = (const char*)::memchr(pDecrypted,'\0',DecryptedSize)) {
            DecryptedSize = pNullChar - pDecrypted;
         }
         Output.write( pDecrypted, DecryptedSize );
      }
   } else {
      Valid = false;
   }

   if (!Valid) {      
      Output = "Could not decrypt data. The key or data may be incorrect.";
   }
   return Valid;
}

bool DBEdecryptString(const COLstring& InputData, COLstring* pOutput) {
   DBEencryptionKey Key;
   ::memcpy(Key.Value, DEFAULT_KEY, KEY_SIZE);
   return DBEdecryptString(InputData, Key, pOutput);
}

