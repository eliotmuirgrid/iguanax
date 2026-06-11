//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLdll
//
// Description:
//
// Implementation
//
// Author: Eliot Muir, Rishi Marwah, Guillaume and Eric should get credit (blame!?) too
// Date:   Tue 09/01/1998 
//---------------------------------------------------------------------------
#include "COLdll.h"
#include "COLerror.h"
#include "COLstring.h"
#include "COLhashmap.h"
#include "COLlog.h"
COL_LOG_MODULE;

#include <dlfcn.h>
#ifdef __APPLE__
   #include <mach-o/dyld.h>
#endif
#include <unistd.h>

#ifdef BUGGY_DLOPEN_IMPLEMENTATION
#include <pthread.h>
#endif // BUGGY_DLOPEN_IMPLEMENTATION

#define COL_DLL_NO_ERROR_FROM_LOAD 0
#define COLDLLfileExistsPermissions F_OK

bool COLdllHasPermission(const COLstring& FileName, COLuint32 PermissionMode){
    COL_FUNCTION(COLdllHasPermission);
    return (access(FileName.c_str(), PermissionMode) == 0);
}

// This is staying as a helper function because it encapsulates all of the
// error handling which is quite fragile.
static void COLhandleError(const COLstring& Message, const char* dlerrorString = ""){
   // The way dlerror works is to give you a string for the last error that
   // occurred since the last time you called dlerror. What this means is
   // that once an error occurs that you want to report, you can only call
   // dlerror once. See ticket #8310 for more details.
   //
   // Note: this function used to query errno - that was incorrect.
   //       errno is not guarenteed to be set for errors in
   //       dlopen(), dlsym() and dlcose() in all UNIXes.
   COL_ERROR_STREAM_PLAIN(Message << newline << dlerrorString, COLerror::SystemError);
}

COLdll::COLdll() :  DllHandle(NULL), IsOwner(false){
   COL_METHOD(COLdll::COLdll);
}

COLdll::COLdll(const COLstring& DllFileName, bool IsOwner) : DllHandle(NULL),IsOwner(false){
   COL_METHOD(COLdll::COLdll);
   COL_VAR2(DllFileName, IsOwner);
   loadLibrary(DllFileName, IsOwner);
}

COLdll::~COLdll(){
   COL_METHOD(COLdll::~COLdll);
   try{
      if ( IsOwner ){
         freeLibrary();
      }
   } catch (const COLerror& Error){
      COL_VAR(Error);
   }
}

#ifdef BUGGY_DLOPEN_IMPLEMENTATION

// A mutex to guard s_NotFoundSharedLibraryHash creation and access.
// Note: PTHREAD_MUTEX_INITIALIZER is safer than using a static COLmutex object.
// See "man pthread_mutex_init" for details.
static pthread_mutex_t s_NotFoundSharedLibraryHashMutex = PTHREAD_MUTEX_INITIALIZER;

struct COLpthreadMutexLocker{
   COLpthreadMutexLocker(pthread_mutex_t* mutex) : m_Mutex(mutex) { pthread_mutex_lock(m_Mutex); }
   ~COLpthreadMutexLocker() { pthread_mutex_unlock(m_Mutex); }
   pthread_mutex_t* m_Mutex;
};

// Only to be created/used when locked by s_NotFoundSharedLibraryHashMutex.
// Key is the shared library name, Data is the dlerror() string.
static COLhashmap<COLstring, COLstring>* s_NotFoundSharedLibraryHash = 0;

#endif // BUGGY_DLOPEN_IMPLEMENTATION

// A wrapper for the buggy HP-UX B.11.11 dlopen(). See Ticket #10231.
// Only necessary to retain NOT found shared libraries.
//
static void* dlopenWrapper(const char* SharedLibraryName, COLstring* ErrorString){
COL_FUNCTION(dlopenWrapper);
#ifdef BUGGY_DLOPEN_IMPLEMENTATION
   COL_TRC("BUGGY_DLOPEN_IMPLEMENTATION");
   {
      COLpthreadMutexLocker Locker(&s_NotFoundSharedLibraryHashMutex);
      if (0 == s_NotFoundSharedLibraryHash){
         // leak COLhashmap on purpose on buggy dlopen platforms.
         s_NotFoundSharedLibraryHash = new COLhashmap<COLstring,COLstring>(COLhash<COLstring>::defaultHash);
      }
      COLhashmapPlace Place = s_NotFoundSharedLibraryHash->find(SharedLibraryName);
      if (Place){
         // Failure - shared library name was previously found to return NULL.
         *ErrorString = (*s_NotFoundSharedLibraryHash)[Place];
         return 0;
      }
   }
#endif // BUGGY_DLOPEN_IMPLEMENTATION

   // Shared library was not found in hash map.
 
   ::dlerror(); // Clear any existing error
   // Don't hold mutex during call to dlopen().
   COL_TRC("Calling ::dlopen to get handle");
   void* Handle = ::dlopen(SharedLibraryName, RTLD_LAZY);
   COL_TRC("Done.");
   if (Handle == 0){
      COL_TRC("Handle == 0");
      // Don't hold mutex during call to dlerror().
      *ErrorString = ::dlerror();

#ifdef BUGGY_DLOPEN_IMPLEMENTATION
      COLpthreadMutexLocker Locker(&s_NotFoundSharedLibraryHashMutex);
      s_NotFoundSharedLibraryHash->add(SharedLibraryName, *ErrorString);
#endif // BUGGY_DLOPEN_IMPLEMENTATION
   }
   return Handle;
}

void COLdll::freeLibrary(){
   COL_METHOD(COLdll::freeLibrary);
   if (DllHandle != NULL){
      ::dlerror(); // Clear any existing error
      if(::dlclose(DllHandle) != 0){
         COLhandleError("Failed to free library.", ::dlerror());
      }
      DllHandle = NULL;
   }
}

void COLdll::loadLibrary(const COLstring& DllFileName, bool IsOwnerIn){
   COL_METHOD(COLdll::loadLibrary);
   COL_VAR2(DllFileName, IsOwner);
   // need to free before loading again, not only is it good practice, but
   // dlopen on posix is reference counted
   if ( IsOwner ){
      freeLibrary();
   }

   IsOwner = IsOwnerIn;

   if (DllFileName.is_null()){
      COL_TRC("Unable to load library, NULL file name.");
      COLhandleError("Unable to load library, NULL file name.");
   }
   COLstring ErrorString;
   DllHandle = ::dlopenWrapper(DllFileName.c_str(), &ErrorString);
   if (DllHandle == NULL){
      COL_TRC("Unable to load library: " + DllFileName);
      COLhandleError("Unable to load library: " + DllFileName, ErrorString.c_str());
   }
   LibraryName = DllFileName;
}

bool COLdll::loadLibraryWithStatus (const COLstring& DllFileName, bool IsOwnerIn) {
   COL_METHOD(COLdll::loadLibraryWithStatus);
   COL_VAR2(DllFileName, IsOwner);
   // need to free before loading again, not only is it good practice, but
   // dlopen on posix is reference counted
   if ( IsOwner ){
      freeLibrary();
   }

   IsOwner = IsOwnerIn;

   if (DllFileName.is_null()) {
      COL_TRC("Unable to load library, NULL file name.");
      return false;
   }
   COLstring ErrorString;
   DllHandle = ::dlopenWrapper(DllFileName.c_str(), &ErrorString);
   if (DllHandle == NULL){
      COL_TRC("Unable to load library: " + DllFileName + " Error: " + ErrorString.c_str());
      return false;
   }
   return true;
}

COLdllLoadResult COLdll::tryLoadLibrary(const COLstring& DllFileName, bool IsOwnerIn, COLstring* pError){
   COL_METHOD(COLdll::tryLoadLibrary);
   COL_VAR2(DllFileName, IsOwnerIn);
   // need to free before loading again, not only is it good practice, but
   // dlopen on posix is reference counted
   if ( IsOwner ){
      freeLibrary();
   }
   IsOwner = IsOwnerIn;
   if (DllFileName.is_null()){
      COL_TRC("Unable to load library, NULL file name.");
      return COL_DLL_LOAD_NOT_EXIST;
   }

   COLstring ErrorString;
   DllHandle = ::dlopenWrapper(DllFileName.c_str(), &ErrorString);
   COL_VAR(ErrorString);
   if (DllHandle){
      COL_TRC("Success");
      // success
      LibraryName = DllFileName;
      return COL_DLL_LOAD_OK;
   }
   COL_TRC("Failure");
   LibraryName.clear();
   if(pError) { *pError = ErrorString; }
   return COL_DLL_LOAD_NO_PERMISSIONS;
}

void* COLdll::getProcAddress(const char* ProcName){
   COL_METHOD(COLdll::getProcAddress);
   COLPRECONDITION(DllHandle != NULL);
   // A return value of NULL does not necessarily mean that an error has occurred, as a 
   // DLL can be specially constructed to have a function at NULL such that a NULL return
   // value from dlsym would not indicate a failure. This behavior however is not
   // recommended and as such is not checked for. Check the man page for dlsym for more
   // information.  
   void* pFunction = NULL;
   ::dlerror(); // Clear any existing error
   pFunction = ::dlsym(DllHandle, ProcName);
   COLstring Error = ::dlerror();
   if (!Error.is_null()){
      throw COLerror(COL_T("Failed to load procedure: ")  + COLstring(ProcName) + Error, COL_PROC_DOES_NOT_EXIST);
   }
   return pFunction;
}

bool COLdll::canLoadProcAddress(const char* ProcName){
   COL_METHOD(COLdll::canLoadProcAddress);
   COLPRECONDITION(DllHandle != NULL);
   // A return value of NULL does not necessarily mean that an error has occurred, as a 
   // DLL can be specially constructed to have a function at NULL such that a NULL return
   // value from dlsym would not indicate a failure. This behavior however is not
   // recommended and as such is not checked for. Check the man page for dlsym for more
   // information.  
   ::dlerror(); // Clear any existing error
   return (::dlsym(DllHandle, ProcName) != NULL) ? true : false; 
}

const COLstring& COLdll::libraryName() const{
   COL_METHOD(COLdll::libraryName);
   return LibraryName;
}

bool COLdll::loaded() const{
   COL_METHOD(COLdll::loaded);
   return DllHandle != NULL;
}

const COLstring& COLdll::lastErrorMessage() const {
   COL_METHOD(COLdll::lastErrorMessage);
   return ErrorString;
}

