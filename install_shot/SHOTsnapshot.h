#ifndef __SHOT_SNAPSHOT_H__
#define __SHOT_SNAPSHOT_H__

//---------------------------------------------------------------------------
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SHOTsnapshot
//
// Description:
//
// This class is used to take a snapshot of the current directory (and its
// subdirectories), and output the list of files to a log file.
// 
// Author: Kevin Senn
// Date:   Wednesday, September 14th, 2005 @ 1:57:00 PM
//
//---------------------------------------------------------------------------

class COLstring;
class COLdateTime;

class SHOTsnapshotPrivate;

#define SHOT_WRITE true
#define SHOT_READ false

class SHOTsnapshot
{
public:
   // Create a new SHOTsnapshot object.  An actual snapshot will not be
   // taken until the takeSnapshot() method is called.
   SHOTsnapshot(const COLstring& LogFileName);
   virtual ~SHOTsnapshot();
   
   // Write the snapshot of the current directory to this snapshot's file.
   void takeSnapshot();

   // Open the before and after snapshots, find the differences (that is,
   // which files were installed), and output those differences to this
   // object's logfile, not rewriting it).
   // This will create <install-section>.XXX if it doesn't already exist.
   void diffSnapshot(const COLstring Before, const COLstring After);
   
private:
   SHOTsnapshotPrivate* pMember;

   // Recursive function to write files in the current directory and
   // subdirectories to a log file.  Unless it is being called recursively,
   // be "", since this is the relative path of a the directory that is
   // being enumerated.
   void enumerateFiles(const COLstring Directory);

   // Construct a hash table from the files listed in FileName
   void fillTable(const COLstring FileName);

   /*// Parse the line in the snapshot file specified by LineIndex, and
   // set FilePath to be the path of the file, and TimeStamp to be
   // the file's creation timestamp.
   void readSnapshotEntry
   (
      COLindex     LineIndex,
      COLstring&   FilePath,
      COLdateTime& TimeStamp
   );*/
   
   /*// Check to see if the file specified by FilePath has changed, with
   // respect to TimeStamp (it's creation timestamp).  If a file has
   // been changed or removed, this information will be output.
   void checkForFileChange
   (
      const COLstring FilePath,
      const COLdateTime TimeStamp
   );*/

   SHOTsnapshot(SHOTsnapshot& Orig); // not allowed
   SHOTsnapshot& operator=(SHOTsnapshot& Orig); // not allowed
};

#endif // end of defensive include
