//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKSsslUtils
//
// Author: Kevin Cai
//
// Description: Helper functions for setting up / cleaning SSL library and
//  error messages.
//---------------------------------------------------------------------------


#include <SCKS/SCKSsslUtils.h>
#include <SCKS/SCKSsslContext.h>

#ifdef _WIN32
   #include <SCK/SCKsystem.h>  // for windows.h without the VS2013 warning headaches.
#else
   #include <pthread.h>
#endif

#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/crypto.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// TODO: Custom error messages intead of default SSL errors?
// TODO: Dynamic mutex locks for futureproofing (consult OpenSSL book).
// TODO: Look into forking processes returning same rand numbers.

COLstring SCKSsslError(){
   // Choose to implement own ERR_print_errors instead of having to use a BIO object.
   COL_FUNCTION(SCKSsslError);
   COLstring ErrorString;
   char Buff[256];
   unsigned long int ErrorCode;
   while ((ErrorCode = ERR_get_error())){
      Buff[0] = '\0';
      ERR_error_string(ErrorCode, Buff);
      if (ErrorString != ""){
         ErrorString += '\n';
      }
      ErrorString += Buff;
   }
   return ErrorString;
}

// TODO change to non const pointer for SCKSsslContext
int SCKSsetServerCtx(SCKSsslContext& Ctx, const COLstring& Certificate, const COLstring& PrivateKey){
   COL_FUNCTION(SCKSsetServerCtx);
   Ctx.freeCtx();
   if ((0 != Ctx.init()) ||
       (0 != Ctx.setCertificate(Certificate.c_str())) ||
       (0 != Ctx.setPrivateKey(PrivateKey.c_str()))){
      return -1;
   }
   return 0;
}

int SCKSsetClientCtx(SCKSsslContext& Ctx){
   COL_FUNCTION(SCKSsetClientCtx);
   Ctx.freeCtx();
   Ctx.setServerMode(false);
   if ((0 != Ctx.init()) ||
       (0 != Ctx.setVerify(false))){
      COL_TRC("SCKSsetClientCtx failed - returning -1");
      return -1;
   }
   COL_TRC("SCKSsetClientCtx succeeded - returning -1");
   return 0;
}

//Following is stolen directly from TCPSsslError.cpp
#ifdef _WIN32
static HANDLE* SslLockCs = 0;

static void SslWin32LockingCallback(int mode, int type, const char *file, int line)
{
   if (mode & CRYPTO_LOCK)
   {
      ::WaitForSingleObject(SslLockCs[type],INFINITE);
   }
   else
   {
      ::ReleaseMutex(SslLockCs[type]);
   }
}

static void SCKSsslThreadSetup()
{
   SslLockCs = (HANDLE*)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(HANDLE));
   for (int LockIndex = 0; LockIndex < CRYPTO_num_locks(); LockIndex++)
   {
      SslLockCs[LockIndex] = ::CreateMutex(NULL,FALSE,NULL);
   }
   CRYPTO_set_locking_callback(&SslWin32LockingCallback);
   /* id callback defined */
}

static void SCKSsslThreadCleanup()
{
   if (SslLockCs)
   {
      CRYPTO_set_locking_callback(NULL);
      for (int LockIndex = 0; LockIndex < CRYPTO_num_locks(); LockIndex++)
      {
         CloseHandle(SslLockCs[LockIndex]);
      }
      OPENSSL_free(SslLockCs);
   }
}

#else //_WIN32

static pthread_mutex_t *SslLockCs = 0;
static long* SslLockCount = 0;

void SCKSsslPthreadsLockingCallback(int mode, int type, const char *file, int line) {
   if (mode & CRYPTO_LOCK) {
      pthread_mutex_lock(&(SslLockCs[type]));
      SslLockCount[type]++;
   } else {
      pthread_mutex_unlock(&(SslLockCs[type]));
   }
}

static unsigned long SslPthreadsThreadId(void) {
   return (unsigned long)pthread_self();
}

void SCKSsslThreadSetup(void) {
   SslLockCs = (pthread_mutex_t*)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));
   SslLockCount = (long*)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(long));
   for (int LockIndex = 0; LockIndex < CRYPTO_num_locks(); LockIndex++) {
      SslLockCount[LockIndex] = 0;
      pthread_mutex_init(&(SslLockCs[LockIndex]),NULL);
   }

   CRYPTO_set_id_callback(&SslPthreadsThreadId);
   CRYPTO_set_locking_callback(&SCKSsslPthreadsLockingCallback);
}

void SCKSsslThreadCleanup(void) {
   if (SslLockCs) {
      CRYPTO_set_locking_callback(NULL);
      for (int LockIndex = 0; LockIndex < CRYPTO_num_locks(); LockIndex++) {
         pthread_mutex_destroy(&(SslLockCs[LockIndex]));
      }
      // ERR_remove_thread_state(NULL);
      OPENSSL_free(SslLockCs);
      OPENSSL_free(SslLockCount);
   }
}

#endif

void SCKSinitSSL(){
   COL_FUNCTION(SCKSinitSSL);
   SSL_library_init();
   SSL_load_error_strings();
   OpenSSL_add_all_algorithms();
   SCKSsslThreadSetup();
}

void SCKSfreeSSL(){
   COL_FUNCTION(SCKSfreeSSL);
   SCKSsslThreadCleanup();
   ERR_free_strings();
   EVP_cleanup();
}

SCKSsslInit::SCKSsslInit(){
   SCKSinitSSL();
}

SCKSsslInit::~SCKSsslInit(){
   SCKSfreeSSL();
}
