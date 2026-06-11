#ifndef __FIL_DIR_ENUMERATOR_H__
#define __FIL_DIR_ENUMERATOR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILdirEnumerator
//
// Description:
//
// Class which enumerates files in a directory.  Couple of 'gotchas' for
// POSIX and _WIN32.
//
// For _WIN32 "*.*" will give all the files in a directory.
// For POSIX "*" will give all the files in a directory - except for the hidden ones.
// For POSIX ".*" will give all the hidden files in a directory.
//
// This makes for a fun time with writing portable routines that use this class.
//
// Author: Eliot Muir
// Date:   Tue 09/01/1998 
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include "FILutils.h"

class COLstring;
class FILdirEnumeratorPrivate;
class COLdateTime;

extern const char* FIL_ANYFILE;  // *.* for _WIN32 and * for POSIX.

class FILdirEnumerator{
public:
   // A good DirName is "*.*" if you are starting out with this library under _WIN32
   // See the main comment for problems with the DirName...
   FILdirEnumerator(const COLstring& DirName, bool DirectoriesOnly=false, bool WantSort=true);
   FILdirEnumerator(const COLstring& DirName, FILnameFilter pFilter, bool DirectoriesOnly=false, bool WantSort=true);
   ~FILdirEnumerator();

   void moveToFirst();

   // in theory you should be able to receive the name of each file here.
   bool getFile(COLstring& FileName);  

   // These functions operate on the last file
   bool isDirectory() const;
   bool isReadOnly() const;
   bool isHidden() const;
   
   const COLdateTime& creationTime() const;
   const COLdateTime& lastWriteTime() const;

   COLuint64 fileSize() const;
private:
   FILdirEnumeratorPrivate* pMember;
   FILdirEnumerator(const FILdirEnumerator& Orig);
   FILdirEnumerator& operator=(const FILdirEnumerator& Orig);
};

#endif // end of defensive include

