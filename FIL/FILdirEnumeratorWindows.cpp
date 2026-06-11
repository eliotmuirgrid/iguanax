//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILdirEnumerator
//
// Description:
//
// Class which enumerates files in a directory.
//
// Author: Eliot Muir
// Date:   Tue 09/01/1998 
//
//---------------------------------------------------------------------------
#include "FILprecomp.h"
#pragma hdrstop

#include "FILdirEnumerator.h"
#include "FILutils.h"
#include <COL/COLioCheck.h>
#include <COL/COLstring.h>
#include <COL/COLerror.h>
#include <COL/COLostream.h>
#define CHM_FILETIME
#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

const char* FIL_ANYFILE = "*.*";

class FILdirEnumeratorPrivate 
{
public:
   FILdirEnumeratorPrivate(const COLstring& DirName, bool DirOnly);
   ~FILdirEnumeratorPrivate();
   void moveToFirst();
   bool file(COLstring& FileName);

   void setFilter(FILnameFilter pFilter);

   COLstring m_FileExpression;
   COLstring m_WinFileExpression;
   bool m_DirectoriesOnly;
   WIN32_FIND_DATAW m_FindDataStruct;
   bool m_FileExists;
   HANDLE m_pSearchHandle;

   bool m_OnFirstFile;

   bool isDirectory() const;
   void nextFile();

   COLdateTime m_CreationTimeStamp;
   COLdateTime m_LastWriteTimeStamp;

   bool isDotDot();

private:
   FILnameFilter m_pFilter;
};

FILdirEnumeratorPrivate::FILdirEnumeratorPrivate(const COLstring& DirName, bool DirOnly)
: m_pSearchHandle(NULL),
  m_FileExpression(DirName),
  m_DirectoriesOnly(DirOnly)
{
   COL_METHOD(FILdirEnumeratorPrivate::FILdirEnumeratorPrivate);
   FILmangleFileNameForWindows(m_FileExpression, &m_WinFileExpression);
   ZeroMemory(&m_FindDataStruct, sizeof(m_FindDataStruct));   
}

FILdirEnumeratorPrivate::~FILdirEnumeratorPrivate(){
   COL_METHOD(FILdirEnumeratorPrivate::~FILdirEnumeratorPrivate);
   try {
      if (m_pSearchHandle != NULL) {
         COL_CHECK_WIN32_CALL(::FindClose(m_pSearchHandle));
      }
   } catch(const COLerror& Error) {
      COL_ERR(Error);
   }
}

void FILdirEnumeratorPrivate::setFilter(FILnameFilter pFilter) {
   COL_METHOD(FILdirEnumeratorPrivate::setFilter);
   m_pFilter = pFilter;
}

void FILdirEnumeratorPrivate::moveToFirst(){
   COL_METHOD(FILdirEnumeratorPrivate::moveToFirst);
   m_OnFirstFile = true;
   m_FileExists = true;

   if (m_pSearchHandle != NULL) {
      COL_CHECK_WIN32_CALL(::FindClose(m_pSearchHandle));
      m_pSearchHandle = NULL;
   }
   m_FindDataStruct.cFileName[0] = 0;
   m_pSearchHandle = ::FindFirstFileW((wchar_t*)m_WinFileExpression.get_buffer(), &m_FindDataStruct);
   if (m_pSearchHandle == INVALID_HANDLE_VALUE) {
      COL_TRC("INVALID HANDLE VALUE");
      // In this case we do not check the error message
      // deliberately because it means there are no files to
      // enumerate through.  So we should just give these settings:
      m_pSearchHandle = NULL;
      m_FileExists = false;
   } else if (isDotDot() || (m_DirectoriesOnly && !isDirectory())) {
      nextFile();
   }
}

bool FILdirEnumeratorPrivate::isDotDot() {
   COL_METHOD(FILdirEnumeratorPrivate::isDotDot);
   COLstring FileName;
   FILunMangleWindowsFileName(m_FindDataStruct.cFileName, &FileName);
   if (FileName == ".") {
      return true;
   }
   if (FileName == "..") {
      return true;
   }
   return false;
}

void FILdirEnumeratorPrivate::nextFile() {
   COL_METHOD(FILdirEnumeratorPrivate::nextFile);
   bool ReturnCode;
   if (m_DirectoriesOnly) {
      do {
         ReturnCode = ::FindNextFileW(m_pSearchHandle, &m_FindDataStruct);               
      } while ((ReturnCode != 0) && (!isDirectory() || isDotDot()));  
   } else {
      do {
         ReturnCode = ::FindNextFileW(m_pSearchHandle, &m_FindDataStruct);   
         if (ReturnCode == 0) {
            break;
         }
      } while(isDotDot());
   }
   if (ReturnCode != 0) {
      if (m_DirectoriesOnly) {
         m_FileExists = isDirectory();
      } else {
         m_FileExists = true;
      }
      return;
   } else {
      if (::GetLastError() != ERROR_NO_MORE_FILES) {
         COL_THROW_WIN32_ERROR("FindNextFile failed on " << m_FileExpression);
      } else {
         m_FileExists = false;
         return;
      }
   }
}

bool FILdirEnumeratorPrivate::file(COLstring& FileName) {
   COL_METHOD(FILdirEnumeratorPrivate::file);
   if (m_OnFirstFile) {
      m_OnFirstFile = false;
   } else {
      nextFile();
   }

   if (m_FileExists) {
      //FileName = m_FindDataStruct.cFileName;
      FILunMangleWindowsFileName(m_FindDataStruct.cFileName, &FileName);
   }
   return m_FileExists;
}

FILdirEnumerator::FILdirEnumerator(const COLstring& DirName, bool DirectoriesOnly, bool WantSort){
   COL_METHOD(FILdirEnumerator::FILdirEnumerator);
   COLassertIfMainThread();
   COL_VAR3(DirName, DirectoriesOnly, WantSort);
   pMember = new FILdirEnumeratorPrivate(DirName, DirectoriesOnly);
   moveToFirst();
}

// At the moment, there is no Windows use for name filters in these enumerators, so we do nothing with them.
// Over in POSIX, they actually run.
FILdirEnumerator::FILdirEnumerator(const COLstring& DirName, FILnameFilter pFilter, bool DirectoriesOnly, bool WantSort) {
   COL_METHOD(FILdirEnumerator::FILdirEnumerator);
   COL_VAR3(DirName, DirectoriesOnly, WantSort);
   pMember = new FILdirEnumeratorPrivate(DirName, DirectoriesOnly);
   moveToFirst();
}

FILdirEnumerator::~FILdirEnumerator(){
   COL_METHOD(FILdirEnumerator::~FILdirEnumerator);
   delete pMember;
}

void FILdirEnumerator::moveToFirst() {
   COL_METHOD(FILdirEnumerator::moveToFirst);
   pMember->moveToFirst();
}

bool FILdirEnumerator::getFile(COLstring& FileName) {
   COL_METHOD(FILdirEnumerator::getFile);
   COL_VAR(FileName);
   return pMember->file(FileName);
}

bool FILdirEnumeratorPrivate::isDirectory() const {
   COL_METHOD(FILdirEnumeratorPrivate::isDirectory);
   return (FILE_ATTRIBUTE_DIRECTORY == (m_FindDataStruct.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

bool FILdirEnumerator::isDirectory() const {
   COL_METHOD(FILdirEnumerator::isDirectory);
   return pMember->isDirectory();
}

bool FILdirEnumerator::isReadOnly() const {
   COL_METHOD(FILdirEnumerator::isReadOnly);
   return (FILE_ATTRIBUTE_READONLY == (pMember->m_FindDataStruct.dwFileAttributes & FILE_ATTRIBUTE_READONLY));
}

bool FILdirEnumerator::isHidden() const {
   COL_METHOD(FILdirEnumerator::isHidden);
   return (FILE_ATTRIBUTE_HIDDEN == (pMember->m_FindDataStruct.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN));
}

const COLdateTime& FILdirEnumerator::creationTime() const {
   COL_METHOD(FILdirEnumerator::creationTime);
   pMember->m_CreationTimeStamp = pMember->m_FindDataStruct.ftCreationTime;
   return pMember->m_CreationTimeStamp;   
}

const COLdateTime& FILdirEnumerator::lastWriteTime() const {
   COL_METHOD(FILdirEnumerator::lastWriteTime);
   pMember->m_LastWriteTimeStamp = pMember->m_FindDataStruct.ftLastWriteTime;
   return pMember->m_LastWriteTimeStamp;
}

COLuint64 FILdirEnumerator::fileSize() const {
   COL_METHOD(FILdirEnumerator::fileSize);
   return ((COLuint64)pMember->m_FindDataStruct.nFileSizeHigh << 32) + pMember->m_FindDataStruct.nFileSizeLow;
}
