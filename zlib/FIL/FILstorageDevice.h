#ifndef __FIL_STORAGE_DEVICE_H__
#define __FIL_STORAGE_DEVICE_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILstorageDevice
//
// Description:
//
// This class together with FILstorageManager is intended to provide
// an interface to the operating system calls for finding out the
// drives (mounts) available to an application, what type they are and
// how much storage space is available.
//
// WARNING!  This class uses functions not available in early versions
// of Windows 95.  We could make the class smarter if this becomes
// important.  I think this is not a priority at this point though.
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004 
//
//
//---------------------------------------------------------------------------


#include <COL/COLminimumInclude.h>

class FILstorageDevicePrivate;

class FILstorageDevice
{
public:
   // DevicePath will be something like "D:"
   FILstorageDevice(const COLstring& DevicePath);
   ~FILstorageDevice();


   const COLstring& path() const;

   // Volume label
   const COLstring& label() const;
   void setLabel(const COLstring& Name);


   enum FILstorageType
   {
      CdRom=0,
      Fixed=1,
      RamDisc=2,
      Network=3,
      Unknown=4,
      Removable=5
   };

   // What type of device is this
   FILstorageType type() const;
  
   // Name of the file system - i.e. FAT32, NTFS etc.
   const COLstring& fileSystemName() const;

   // Serial number of drive
   COLuint32 serialNumber() const;

   COLuint32 maximumFileNameLength() const;

   // returns the amount of space available to the user associated
   // with the calling thread.
   COLuint64 spaceFreeForMe() const;

   // returns the total amount of space available on the disk
   COLuint64 spaceFreeForEveryone() const;

   // returns the amount of data on the disk as seen by the user associated
   // with the calling thread.
   COLuint64 spaceUsed() const;
private:

   FILstorageDevicePrivate* pMember;

   FILstorageDevice(const FILstorageDevice& Orig); // not allowed
   FILstorageDevice& operator=(const FILstorageDevice& Orig); // not allowed
};

#endif // end of defensive include
