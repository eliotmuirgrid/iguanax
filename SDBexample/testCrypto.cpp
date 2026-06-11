// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCrypto
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Tue Oct  1 10:36:34 EDT 2024
// ---------------------------------------------------------------------------
#include <openssl/rand.h>

#include "testCrypto.h"

#include <UNIT/UNITapp.h>

#include <SDBT/SDBTcrypto.h>

#include <SDB/SDBcrypto.h>
#include <SDB/SDBcryptoFile.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testEncryptDecrypt() {
   COL_FUNCTION(testEncryptDecrypt);

   COLstring Key(SDB_AES_GCM_SIZE_KEY, '\0');
   COLstring IV(SDB_AES_GCM_SIZE_IV, '\0');
   COLstring Plaintext = "Lorem ipsum odor amet, consectetuer adipiscing elit.";
   COLstring AAD = "AAD";

   // Generate random key and IV
   RAND_bytes((unsigned char*) Key.data(), Key.size());
   RAND_bytes((unsigned char*) IV.data(), IV.size());

   COLstring Ciphertext;
   COLstring Tag;

   COL_TRC("TEST - Encrypt and decrypt should return the original Plaintext");
   UNIT_ASSERT(SDBTencrypt(Plaintext, IV, Key, &Ciphertext, &Tag, AAD));
   COLstring Result;
   UNIT_ASSERT(SDBTdecrypt(Ciphertext, Tag, IV, Key, &Result, AAD));
   UNIT_ASSERT_EQUALS(Plaintext, Result);

   COL_TRC("TEST - Omitting the AAD should cause the decryption to fail");
   UNIT_ASSERT(!SDBTdecrypt(Ciphertext, Tag, IV, Key, &Result));
}

static void testPasswordEncryptDecrypt() {
   COL_FUNCTION(testPasswordEncryptDecrypt);
   COLstring Plaintext = "Pharetra auctor faucibus feugiat quam posuere nam.";
   COLstring Salt(SDB_PBKDF2_SIZE_SALT, '\0');
   COLstring KDK = "Iguana-Service";

   RAND_bytes((unsigned char*) Salt.data(), Salt.size());

   COL_TRC("TEST - Encrypt and decrypt should return the original Plaintext");
   COLstring Ciphertext, Tag, IV;
   UNIT_ASSERT(SDBpasswordEncrypt(Plaintext, KDK, Salt, &Ciphertext, &Tag, &IV));
   COLstring Result;
   UNIT_ASSERT(SDBpasswordDecrypt(Ciphertext, Tag, IV, KDK, Salt, &Result));
   UNIT_ASSERT_EQUALS(Plaintext, Result);
}

static void SDBTencrypt(EVP_CIPHER_CTX *ctx, const COLstring& Plaintext, COLstring* pCiphertext) {
   COL_FUNCTION(SDBTencrypt);
   int Length = 0;
   pCiphertext->setCapacity(Plaintext.size());
   pCiphertext->setSize(Plaintext.size());
   EVP_EncryptUpdate(ctx, (unsigned char*)pCiphertext->get_buffer(), &Length, (const unsigned char*)Plaintext.data(), Plaintext.size());
   UNIT_ASSERT_EQUALS(Plaintext.size(), Length);
}

#include <COL/COLarray.h>
static void testCounter() {
   COL_FUNCTION(testCounter);
   COLstring key(SDB_AES_GCM_SIZE_KEY, '\0');
   COLstring iv(SDB_AES_GCM_SIZE_IV, '\0');
   RAND_bytes((unsigned char*) key.data(), key.size());
   RAND_bytes((unsigned char*) iv.data(), iv.size());

   // Test - encrypt 3 msgs in session 1. encrypt 4th msg in session 2. Decrypt concatenated ciphertexts in a single session
   EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
   EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, (const unsigned char*)key.data(), (const unsigned char*)iv.data());

   COLstring Encrypted;
   COLarray<COLstring> MessageList;
   MessageList.push_back("First message to encrypt");
   MessageList.push_back("Second message in the stream");
   MessageList.push_back("Another message here");
   for (int i = 0; i < MessageList.size(); ++i) {
      COLstring Ciphertext;
      SDBTencrypt(ctx, MessageList[i], &Ciphertext);
      UNIT_ASSERT_EQUALS(MessageList[i].size(), Ciphertext.size());
      Encrypted.append(Ciphertext);
   }

   int Length = -1;
   COLstring Final;
   EVP_EncryptFinal_ex(ctx, (unsigned char*)Final.get_buffer(), &Length);
   UNIT_ASSERT_EQUALS(0, Length);
   EVP_CIPHER_CTX_free(ctx);

   ctx = EVP_CIPHER_CTX_new();
   EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, (const unsigned char*)key.data(), (const unsigned char*)iv.data());
   COLstring Ciphertext;
   MessageList.push_back("Yet another message");
   SDBTencrypt(ctx, MessageList.back(), &Ciphertext);
   UNIT_ASSERT_EQUALS(MessageList.back().size(), Ciphertext.size());
   Encrypted.append(Ciphertext);
   EVP_EncryptFinal_ex(ctx, (unsigned char*)Final.get_buffer(), &Length);
   UNIT_ASSERT_EQUALS(0, Length);
   EVP_CIPHER_CTX_free(ctx);

   // decrypt concatenated ciphertext using a single decryption session
   COLstring Plaintext(Encrypted.size(), '\0');
   ctx = EVP_CIPHER_CTX_new();
   EVP_DecryptInit_ex(ctx, EVP_aes_128_ctr(), nullptr, (const unsigned char*) key.data(), (const unsigned char*) iv.data());
   EVP_DecryptUpdate(ctx, (unsigned char*)Plaintext.get_buffer(), &Length, (const unsigned char*)Encrypted.data(), Encrypted.size());
   UNIT_ASSERT_EQUALS(Encrypted.size(), Length);
   EVP_DecryptFinal_ex(ctx, (unsigned char*)Final.get_buffer(), &Length);
   UNIT_ASSERT_EQUALS(0, Length);
   EVP_CIPHER_CTX_free(ctx);

   COLcout << "Original  plaintext: ";
   for (const auto& Msg : MessageList) {
      COLcout << Msg;
   }
   COLcout << newline;
   COLcout << "Decrypted plaintext: ";
   COLcout << Plaintext << newline;

   // This doesn't work because the first 3 messages were encrypted using counter say 1, 2, 3, and the 4th used 1.
   // Decryption used counter 1, 2, 3, 4 for the 4 messages. So you can't concatenate ciphertext from different
   // sessions together and expect them to decrypt properly in a single session.
}

#include <chrono>
static void testEncryptSpeed() {
   COL_FUNCTION(testEncryptSpeed);
   COLstring Plaintext(131072, '@');
   COLstring IV(SDB_AES_GCM_SIZE_IV, '\0');
   COLstring Key(SDB_AES_GCM_SIZE_KEY, '\0');
   RAND_bytes((unsigned char*) Key.data(), Key.size());
   int MessageCount = 10000;

   // TEST - 10000 enryption sessions for 10000 messages
   auto Start = std::chrono::high_resolution_clock::now();
   COLstring Ciphertext, Tag;
   for (int i = 0; i < MessageCount; ++i) {
      RAND_bytes((unsigned char*) IV.data(), IV.size());
      SDBencrypt(Plaintext, IV, Key, &Ciphertext, &Tag);
   }
   auto End = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> Duration = End - Start;
   COLcout << "Took " << Duration.count() * 1000 << " ms to individually encrypt " << MessageCount << " messages." <<  newline;

   // TEST - 1 enryption session for 10000 messages
   Start = std::chrono::high_resolution_clock::now();
   int Length;
   EVP_CIPHER_CTX *Ctx = EVP_CIPHER_CTX_new();
   EVP_CIPHER_CTX_ctrl(Ctx, EVP_CTRL_GCM_SET_IVLEN, SDB_AES_GCM_SIZE_IV, nullptr);
   EVP_EncryptInit_ex(Ctx, nullptr, nullptr, (const unsigned char*) Key.data(), (const unsigned char*) IV.data());
   for (int i = 0; i < MessageCount; ++i) {
      EVP_EncryptUpdate(Ctx, (unsigned char*) Ciphertext.get_buffer(), &Length, (const unsigned char*) Plaintext.data(), Plaintext.size());
   }
   EVP_EncryptFinal_ex(Ctx, (unsigned char*) Ciphertext.get_buffer(), &Length);
   EVP_CIPHER_CTX_free(Ctx);
   End = std::chrono::high_resolution_clock::now();
   Duration = End - Start;
   COLcout << "Took " << Duration.count() * 1000 << " ms to batch encrypt " << MessageCount << " messages." <<  newline;
}

static void testEncDec() {
   COL_FUNCTION(testEncDec);

   COLstring Key(SDB_AES_GCM_SIZE_KEY, '\0');
   COLstring Plaintext = "Lorem ipsum odor amet, consectetuer adipiscing elit.";
   COLstring AAD = "AAD";

   // Generate random key
   RAND_bytes((unsigned char*) Key.data(), Key.size());

   COLstring Ciphertext;
   COLstring Tag, IV;

   COL_TRC("TEST - Encrypt and decrypt should return the original Plaintext");
   UNIT_ASSERT(SDBencrypt(Plaintext, Key, IV, &Ciphertext, &Tag, AAD));
   COLstring Result;
   UNIT_ASSERT(SDBdecrypt(Ciphertext, Tag, IV, Key, &Result, AAD));
   UNIT_ASSERT_EQUALS(Plaintext, Result);

   COL_TRC("TEST - Omitting the AAD should cause the decryption to fail");
   UNIT_ASSERT(!SDBdecrypt(Ciphertext, Tag, IV, Key, &Result));

   COL_TRC("TEST - Wrong IV should cause the decryption to fail");
   UNIT_ASSERT(!SDBdecrypt(Ciphertext, Tag, "badInitializationVector", Key, &Result, AAD));

   COL_TRC("TEST - Wrong authentication tag should cause the decryption to fail");
   UNIT_ASSERT(!SDBdecrypt(Ciphertext, "wrongTag", IV, Key, &Result, AAD));

   COL_TRC("TEST - Wrong encryption key should cause the decryption to fail");
   UNIT_ASSERT(!SDBdecrypt(Ciphertext, Tag, IV, "wrongKey", &Result, AAD));

   COL_TRC("TEST - Corrupted ciphertext should cause the decryption to fail");
   Ciphertext[Ciphertext.size() / 2] += 32;
   UNIT_ASSERT(!SDBdecrypt(Ciphertext, Tag, IV, Key, &Result, AAD));
}

static void testSerialize() {
   COL_FUNCTION(testSerialize);
   COLstring EncKey("0123456789ABCDEF0123456789ABCDEF");
   COLstring Solution("kajflasjflajsdf");
   COLstring Flatwire;
   SDBencryptAndSerialize(Solution, EncKey, &Flatwire);
   COL_DUMP("Flatwire", Flatwire.data(), Flatwire.size());
   COLstring Answer;
   int Amount = SDBparseAndDecrypt(Flatwire.data(), Flatwire.size(), EncKey, &Answer);
   UNIT_ASSERT_EQUALS(Flatwire.size(), Amount);
   UNIT_ASSERT_EQUALS(Solution, Answer);

   // TEST - return -1 (more data) for a partial encrypted packet
   UNIT_ASSERT_EQUALS(-1, SDBparseAndDecrypt(Flatwire.data(), Flatwire.size() / 2, EncKey, &Answer));
}


void testCrypto(UNITapp* pApp){
   COL_FUNCTION(testCrypto);
   pApp->add("crypto/encrypt/decrypt", &testEncryptDecrypt);
   pApp->add("crypto/password/encrypt/decrypt", &testPasswordEncryptDecrypt);
   pApp->add("crypto/ctr", &testCounter);
   pApp->add("crypto/encrypt/speed", &testEncryptSpeed);
   pApp->add("crypto/enc/dec", &testEncDec);
   pApp->add("crypto/serialize", testSerialize);
}
