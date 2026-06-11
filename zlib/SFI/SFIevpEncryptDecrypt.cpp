//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIevpEncryptDecrypt
//
// Description: 
//
//EVP Symmetric Encryption and Decryption
//
// https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption#C.2B.2B_Programs
// 
// Author: Shuo Hou
// Date:   Monday, March 28th, 2022
//---------------------------------------------------------------------------
#include "SFIevpEncryptDecrypt.h"
#include <openssl/evp.h>
#include <openssl/err.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void handleErrors(void){
   char buf[128];
   ERR_error_string_n(ERR_get_error(),buf,128);
   COLstring Error(buf);
   COL_VAR(Error);
}
// 256 bit key and 128 bit IV
void SFIevpEncrypt(const COLstring& PlainText, const COLstring& Key, const COLstring& IV, COLstring* pCipherText){
   COL_FUNCTION(SFIevpEncrypt);
   EVP_CIPHER_CTX *ctx;

   int len;

   int ciphertext_len;

   /* Create and initialise the context */
   if(!(ctx = EVP_CIPHER_CTX_new()))
      handleErrors();

   /*
   * Initialise the encryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits
   */
   if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)Key.c_str(), (const unsigned char*)IV.c_str()))
      handleErrors();

   int BlockSize = EVP_CIPHER_CTX_block_size(ctx);
   pCipherText->setCapacity(PlainText.size() + BlockSize);
   /*
   * Provide the message to be encrypted, and obtain the encrypted output.
   * EVP_EncryptUpdate can be called multiple times if necessary
   */
   if(1 != EVP_EncryptUpdate(ctx, (unsigned char*)pCipherText->get_buffer(), &len, (const unsigned char*)PlainText.c_str(), PlainText.size()))
      handleErrors();
   ciphertext_len = len;

   /*
   * Finalise the encryption. Further ciphertext bytes may be written at
   * this stage.
   */
   if(1 != EVP_EncryptFinal_ex(ctx, (unsigned char*)pCipherText->get_buffer() + len, &len))
      handleErrors();
   ciphertext_len += len;

   /* Clean up */
   EVP_CIPHER_CTX_free(ctx);

   pCipherText->setSize(ciphertext_len);
}


void SFIevpDecrypt(const COLstring& CipherText, const COLstring& Key, const COLstring& IV, COLstring* pPlainText){
   COL_FUNCTION(SFIevpDecrypt);
   EVP_CIPHER_CTX *ctx;

   int len;

   int plaintext_len;

   /* Create and initialise the context */
   if(!(ctx = EVP_CIPHER_CTX_new()))
      handleErrors();

   /*
   * Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits
   */
   if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)Key.c_str(), (const unsigned char*)IV.c_str()))
      handleErrors();

   int BlockSize = EVP_CIPHER_CTX_block_size(ctx);
   pPlainText->setCapacity(CipherText.size() + BlockSize);
   /*
   * Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary.
   */
   if(1 != EVP_DecryptUpdate(ctx, (unsigned char*)pPlainText->get_buffer(), &len, (const unsigned char*)CipherText.c_str(), CipherText.size()))
      handleErrors();
   plaintext_len = len;

   /*
   * Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
   if(1 != EVP_DecryptFinal_ex(ctx, (unsigned char*) pPlainText->get_buffer() + len, &len))
      handleErrors();
   plaintext_len += len;

   /* Clean up */
   EVP_CIPHER_CTX_free(ctx);

   pPlainText->setSize(plaintext_len);
}