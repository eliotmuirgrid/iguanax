//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
#include "FILprecomp.h"
#pragma hdrstop

#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLioCheck.h>
#include <COL/COLerror.h>
#include <COL/COLostream.h>
#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <glob.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

const char* FIL_ANYFILE = "*";

class FILdirEnumeratorPrivate {
public:
   FILdirEnumeratorPrivate(const COLstring& DirName, bool DirOnly, bool WantSort);
   ~FILdirEnumeratorPrivate();
   void moveToFirst();
   bool getFile(COLstring& FileName);
   bool updateFileInfo();

   void setFilter(FILnameFilter pFilter);

   COLdateTime CreationTimeStamp;
   COLdateTime LastWriteTimeStamp;

   glob_t GlobFileArray;
   COLuint32 FileIndex;

   bool DirectoriesOnly;
   bool IsSorted;
   bool isDirectory();

   struct stat FileInfo;

private:
   FILnameFilter m_pFilter;
};

FILdirEnumeratorPrivate::FILdirEnumeratorPrivate(const COLstring& DirName, bool DirOnly, bool WantSort)
: FileIndex(0), DirectoriesOnly(DirOnly), IsSorted(WantSort)
{
   COL_METHOD(FILdirEnumeratorPrivate::FILdirEnumeratorPrivate);
   int Flags = GLOB_ERR;

   if (!IsSorted) {
      // no sort is 10 times faster than sorted
      Flags |= GLOB_NOSORT;
   }
   memset(&FileInfo, 0, sizeof(FileInfo));

   // Note that I am not bothering with the error callback on glob
   // seems a little complicated but might need to implement this later. 
   int ReturnCode = ::glob(DirName.c_str(), Flags, NULL, &GlobFileArray);

   if (0 != ReturnCode) {
    #ifndef __APPLE__
      if (GLOB_NOMATCH == ReturnCode||GLOB_ABORTED == ReturnCode) {
         // no problem
      } else {
         COL_THROW_STRERROR("glob on " << DirName << " failed.");
      }
    #endif
   }
}

FILdirEnumeratorPrivate::~FILdirEnumeratorPrivate() {
   COL_METHOD(FILdirEnumeratorPrivate::~FILdirEnumeratorPrivate);
   globfree(&GlobFileArray);
} 

void FILdirEnumeratorPrivate::setFilter(FILnameFilter pFilter) {
   COL_METHOD(FILdirEnumeratorPrivate::setFilter);
   m_pFilter = pFilter;
}

bool FILdirEnumeratorPrivate::updateFileInfo() {
   COL_METHOD(FILdirEnumeratorPrivate::updateFileInfo);
   if (FileIndex < GlobFileArray.gl_pathc && !stat(GlobFileArray.gl_pathv[FileIndex],&FileInfo)) {
      LastWriteTimeStamp = COLdateTime(FileInfo.st_mtime);
      CreationTimeStamp = COLdateTime(FileInfo.st_ctime);
      return true;
   }
   return false;
}

bool FILdirEnumeratorPrivate::isDirectory() {
   return S_ISDIR(FileInfo.st_mode) != 0;
}

bool FILdirEnumeratorPrivate::getFile(COLstring& FileName) {
   COL_METHOD(FILdirEnumeratorPrivate::getFile);
   COL_VAR(FileName);

   if (DirectoriesOnly) { // Skip non-directories and files we can't stat().
      while( FileIndex < GlobFileArray.gl_pathc && (!updateFileInfo() || !isDirectory()) ) {
         FileIndex++;
      }
   } else { // Just skip files we can't stat().
      while( FileIndex < GlobFileArray.gl_pathc && !updateFileInfo() ) {
         FileIndex++;
      }
   }

   if (FileIndex < GlobFileArray.gl_pathc) {
      COLstring PathAndName = GlobFileArray.gl_pathv[FileIndex];
      COL_VAR(PathAndName);
      if (m_pFilter.get()) {
         COLstring Tmp;
         m_pFilter->run(PathAndName, &Tmp);
         PathAndName.swap(&Tmp);
      }
      FileName = FILpathName(PathAndName);
      COL_VAR(FileName);
      FileIndex++;
      return true;
   } else {
      return false;
   }
}

void FILdirEnumeratorPrivate::moveToFirst() {
   FileIndex = 0;
}

FILdirEnumerator::FILdirEnumerator(const COLstring& DirName, bool DirectoriesOnly, bool WantSort) {
   COL_METHOD(FILdirEnumerator::FILdirEnumerator);
   COL_VAR3(DirName, DirectoriesOnly, WantSort);
   pMember = new FILdirEnumeratorPrivate(DirName, DirectoriesOnly, WantSort);
   moveToFirst();
}

FILdirEnumerator::FILdirEnumerator(const COLstring& DirName, FILnameFilter pFilter, bool DirectoriesOnly, bool WantSort) {
   COL_METHOD(FILdirEnumerator::FILdirEnumerator);
   COLassertIfMainThread();
   COL_VAR3(DirName, DirectoriesOnly, WantSort);
   pMember = new FILdirEnumeratorPrivate(DirName, DirectoriesOnly, WantSort);
   pMember->setFilter(pFilter);
   moveToFirst();
}

FILdirEnumerator::~FILdirEnumerator(){
   COL_METHOD(FILdirEnumerator::~FILdirEnumerator);
   delete pMember;
}

void FILdirEnumerator::moveToFirst(){ 
   pMember->moveToFirst();
}

// a little bit messy but it should do the job?
bool FILdirEnumerator::getFile(COLstring& FileName) {
   COL_METHOD(FILdirEnumerator::getFile);
   COL_VAR(FileName);
   bool HasFile = true;
   do {
      HasFile = pMember->getFile(FileName);
      if (FileName != "." && FileName != "..") {
         return HasFile;
      }
   } while (HasFile);
   COLPOSTCONDITION(HasFile == false);
   return false;
}

bool FILdirEnumerator::isDirectory() const {
   return pMember->isDirectory();
}

bool FILdirEnumerator::isReadOnly() const {
   // isReadOnly if user has read access but no write access
   return (!(pMember->FileInfo.st_mode & S_IWUSR)) && 
          (pMember->FileInfo.st_mode & S_IRUSR);
}

bool FILdirEnumerator::isHidden() const {
   return false;
}

const COLdateTime& FILdirEnumerator::creationTime() const {
   return pMember->CreationTimeStamp;
}

const COLdateTime& FILdirEnumerator::lastWriteTime() const {
   return pMember->LastWriteTimeStamp;
}

COLuint64 FILdirEnumerator::fileSize() const {
   return pMember->FileInfo.st_size;
}
