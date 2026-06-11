//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SHOTsnapshot
//
// Description:
//
// Implementation
// 
// Author: Kevin Senn
// Date:   Wednesday, September 14th, 2005 @ 1:57:00 PM
//
//---------------------------------------------------------------------------

#include "Precomp.h"
#pragma hdrstop

#include <COL/COLdateTime.h>
#include <FIL/FILutils.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILfile.h>
#include <FIL/FILtextFile.h>
#include <FIL/FILpathUtils.h>
#include <COL/COLhashmap.h>
#include <install_shot/SHOTsnapshot.h>

#define SHOT_ALL_FILES "*"
#define SHOT_INCLUDE_DIRECTORIES false
#define SHOT_DATE_WRITE_FORMAT "%Y-%m-%d %H-%M-%S"
#define SHOT_DATE_READ_FORMAT "%04d-%02d-%02d %02d-%02d-%02d"
#define SHOT_FILE_PATH_INDEX 0
#define SHOT_TIME_STAMP_INDEX 1
#define SHOT_HASH_DUMMY_ITEM 0

class SHOTsnapshotPrivate
{
public:
   COLstring LogFileName;
   FILtextFile LogFile;
   COLhashmap<COLstring, COLuint32> BeforeTable;

   SHOTsnapshotPrivate(COLstring iLogFileName)
   {
      LogFileName = iLogFileName;
   }
   ~SHOTsnapshotPrivate()
   {
      LogFile.close();
   }
private:
   SHOTsnapshotPrivate(SHOTsnapshotPrivate& Orig); // not allowed
   SHOTsnapshotPrivate& operator=(SHOTsnapshotPrivate& Orig); // not allowed
};


SHOTsnapshot::SHOTsnapshot(const COLstring& LogFileName)
{
   pMember = new SHOTsnapshotPrivate(LogFileName);
}

SHOTsnapshot::~SHOTsnapshot()
{
   delete pMember;
}

void SHOTsnapshot::takeSnapshot()
{
   pMember->LogFile.open(pMember->LogFileName, FILfile::Rewrite);
   enumerateFiles("");
}

//  * Open the "Before" file in read mode, and put all filenames into a
//    hash table, and close the file.
//  * Open the file associated with this SHOTsnapshot object in append
//    mode (call it the "Output" file).
//  * Open the "After" file in read mode.
//  * Iterate through the lines in the "After" file, looking each one
//    up in the "Before" hash table. Any filenames that aren't found will
//    be written to the "Output" file.
//  * The "After" file and "Output" files are closed.
void SHOTsnapshot::diffSnapshot(const COLstring Before, const COLstring After)
{
   pMember->LogFile.open(pMember->LogFileName, FILfile::Append);
   // create a lookup table from Before
   COLhashmap<COLstring, COLuint32> Table;
   fillTable(Before);
   FILtextFile AfterFile;
   AfterFile.open(After, FILfile::Read);
   COLstring Line;
   while (AfterFile.readLine(Line))
   {
      if (!pMember->BeforeTable.find(Line))
      {
         pMember->LogFile.writeLine(Line);
      }
   }
}

/////////////////////
// Private methods //
/////////////////////

// Write out all files in the directory.  If we find a subdirectory, we
// will first write out all the contents of it, and then write it's name.
// For example:
// program.exe
// bin\some.dll
// bin\other.dll
// bin
void SHOTsnapshot::enumerateFiles(const COLstring Directory)
{
   COLPRECONDITION(pMember->LogFile.isOpen());
   FILdirEnumerator Enumerator(Directory + SHOT_ALL_FILES, SHOT_INCLUDE_DIRECTORIES);
   COLstring NextFile;
   while (Enumerator.getFile(NextFile))
   {
      if (Enumerator.isDirectory())
      {
         // recursive case: a subdirectory
         SHOT_LOG("Found directory " << NextFile);
         enumerateFiles(Directory + NextFile + FIL_DIR_SEPARATOR);
      }
      
      COLstring Extension = FILpathExtLast(NextFile,false);
      if (Extension == "log")
      {
         continue; // skip this file, since it is a snapshot file
      }
      SHOT_LOG("Printing " << NextFile);
      pMember->LogFile.writeLine(Directory + NextFile);
   }
}

void SHOTsnapshot::fillTable(const COLstring FileName)
{
   FILtextFile File;
   File.open(FileName, FILfile::Read);
   COLstring Line;
   while (File.readLine(Line))
   {
      pMember->BeforeTable.add(Line, SHOT_HASH_DUMMY_ITEM);
   }
   File.close();
}
