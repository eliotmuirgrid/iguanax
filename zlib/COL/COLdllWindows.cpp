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
//
//---------------------------------------------------------------------------
#include "COLwindows.h"
#include "COLdll.h"
#include "COLerror.h"
#include "COLstring.h"
#include "COLlog.h"
COL_LOG_MODULE;

#include <winbase.h>
#include <io.h>

#define COL_WIN32_INVALID_DLL 193
#define COL_WIN32_NO_DLL 126
#define COL_WIN32_NO_PROC 127
#define COLDLLreadPermissions 4

bool COLdllHasPermission(const COLstring& FileName, COLuint32 PermissionMode)
{
   COL_FUNCTION(COLdllHasPermission);
    return (_access(FileName.c_str(), PermissionMode) == 0);
}

COLdll::COLdll() 
:  
   DllHandle(NULL),
   IsOwner(false)
{
}

COLdll::COLdll(const COLstring& DllFileName, bool IsOwner)
:  
   DllHandle(NULL),
   IsOwner(false)
{
   COL_METHOD(COLdll::COLdll);
   loadLibrary(DllFileName, IsOwner);
}

COLdll::~COLdll()
{
   COL_METHOD(COLdll::~COLdll);
   try
   {
      if ( IsOwner )
      {
         freeLibrary();
      }
   }
   catch (const COLerror& Error)
   {
      COL_VAR(Error);
   }
}

void COLdll::freeLibrary()
{
   COL_METHOD(COLdll::freeLibrary);
   if (!DllHandle)
   {
      return; // no library to free;
   }

   if(::FreeLibrary((HINSTANCE)DllHandle) == FALSE)
   {
      // Displaying line and file in this error is pointless.
      COLuint32 ErrorNumber = ::GetLastError();
      throw COLerror(COLerrorString(ErrorNumber), ErrorNumber);
   }
   DllHandle = NULL;
}

void COLdll::loadLibrary
(
   const COLstring& LibraryName,
   bool IsOwnerIn
)
{
   COL_METHOD(COLdll::loadLibrary);
   if (tryLoadLibrary(LibraryName, IsOwnerIn) != COL_DLL_LOAD_OK)
   {
      // Displaying line and file in this error is pointless.
      COLuint32 ErrorNumber = ::GetLastError();
      throw COLerror(COLerrorString(ErrorNumber), ErrorNumber);
   }
}

bool COLdll::loadLibraryWithStatus (const COLstring& LibraryName, bool IsOwnerIn) {
   COL_METHOD(COLdll::loadLibraryWithStatus);
   if (tryLoadLibrary(LibraryName, IsOwnerIn) != COL_DLL_LOAD_OK) {
      // Displaying line and file in this error is pointless.
      COLuint32 ErrorNumber = ::GetLastError();
      COL_TRC("Failed to load dynamic library: " + COLintToString(ErrorNumber));
      return false;
   }
   return true;
}

COLstring COLdllFileName(const COLstring& OriginalFileName, HINSTANCE DllHandle)
{
   COL_FUNCTION(COLdllFileName);
   char CompleteFileName[_MAX_PATH + 1];
   DWORD Length = ::GetModuleFileName(DllHandle, CompleteFileName, _MAX_PATH);
   if (Length && (Length < _MAX_PATH))
   {
      COL_VAR(CompleteFileName);
      return CompleteFileName;
   }
   else
   {
      COL_VAR(OriginalFileName);
      return OriginalFileName;
   }
}

COLdllLoadResult COLdll::tryLoadLibrary
(
   const COLstring& DllFileName,
   bool IsOwnerIn,
   COLstring* pError
)
{  
   COL_METHOD(COLdll::tryLoadLibrary);
   COL_VAR2(DllFileName, IsOwner);
   if ( IsOwner )
   {
      freeLibrary();
   }

   IsOwner = IsOwnerIn;

   // The SetErrorMode call is used to prevent a failure from
   // popping up a cryptic dialog box.
   // See ticket #8156.
   UINT OldErrorMode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
   DllHandle = ::LoadLibraryEx(DllFileName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
   ::SetErrorMode(OldErrorMode);
   COLuint32 LastErrorNumber = (COLuint32)::GetLastError();
   if (DllHandle)
   {
      COL_TRC("DllHandle exists"); 
      LibraryName = COLdllFileName(DllFileName, (HINSTANCE)DllHandle);
      return COL_DLL_LOAD_OK;
   }
   else
   {
      COL_TRC("No Dll Handle"); 
      LibraryName.clear();
      ErrorString = COLerrorString(LastErrorNumber);
      if(pError) { *pError = ErrorString; }
      COL_VAR2(LastErrorNumber, ErrorString); 
      switch (LastErrorNumber)
      {
      case COL_WIN32_INVALID_DLL:
         COL_TRC("Returning invalid library");
         return COL_DLL_LOAD_INVALID_LIBRARY;
      case COL_WIN32_NO_DLL:
         COL_TRC("Returning library does not exist");
         return COL_DLL_LOAD_NOT_EXIST;
      default:
         if(!COLdllHasPermission(DllFileName, COLDLLreadPermissions)) //no error number for invalid file permissions on windows
         {
            COL_TRC("Returning no permissions");
            return COL_DLL_LOAD_NO_PERMISSIONS;
         }
         else
         {
            COL_TRC("Returning unknown error");
            return COL_DLL_LOAD_UNKNOWN_ERROR;
         }
      }
   }
}

void* COLdll::getProcAddress(const char* ProcName)
{
   COL_METHOD(COLdll::getProcAddress);
   COLPRECONDITION(DllHandle != NULL);   
   // expect it to return NULL if the function does not exist.   
   void* pFunction = NULL;
   pFunction = (void*)::GetProcAddress((HINSTANCE)DllHandle, ProcName);

   if (pFunction == NULL)
   {
      COLuint32 LastError = (COLuint32)::GetLastError();
      if (LastError != COL_WIN32_NO_PROC)
      {
         throw COLerror(COLerrorString(LastError) + " while trying to load " + COLstring(ProcName), LastError);
      }
      else
      {
         throw COLerror("Procedure " + COLstring(ProcName) + " not found.", COL_PROC_DOES_NOT_EXIST); 
      }
   }
   return pFunction;
}

bool COLdll::canLoadProcAddress(const char* ProcName)
{
   COL_METHOD(COLdll::canLoadProcAddress);
   return (::GetProcAddress((HINSTANCE)DllHandle, ProcName) != NULL) ? true : false;
}

const COLstring& COLdll::libraryName() const
{
   COL_METHOD(COLdll::libraryName);
   COL_VAR(LibraryName);
   return LibraryName;
}

bool COLdll::loaded() const
{
   COL_METHOD(COLdll::loaded);
   return DllHandle != NULL;
}

const COLstring& COLdll::lastErrorMessage() const
{
   COL_METHOD(COLdll::lastErrorMessage);
   COL_VAR(ErrorString); 
   return ErrorString;
}
