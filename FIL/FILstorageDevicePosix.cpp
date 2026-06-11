//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILstorageDevice
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004 
//
//---------------------------------------------------------------------------
#include "FILprecomp.h"
#pragma hdrstop

#include <COL/COLioCheck.h>

#include <errno.h>
// TODO - Ensure that BSD and Solaris can handle large file systems (64 bits
//        in length).  Linux has been fixed already, it used to over flow when
//        retrieving the size of a device.
#include <FIL/FILstorageDevice.h>
#include <COL/COLerror.h>
#include <stdio.h>
#if defined (__APPLE__)
   #include <fstab.h>
   #include <sys/param.h>
   #include <sys/mount.h>
   #include <sys/ucred.h>
#endif
#ifdef __linux__
   #include <mntent.h>
   #undef __USE_LARGEFILE64
   #define __USE_LARGEFILE64
   #include <sys/vfs.h>
#endif 

class FILstorageDevicePrivate
{
public:
   void mountEntry();

   COLstring DevicePath;
   COLstring Label;
   COLstring FileSystemName;
   COLuint32 SerialNumber;
   COLuint32 MaxFileNameLength;
   COLuint64 SpaceForMe;
   COLuint64 SpaceForEveryOne;
   COLuint64 SpaceUsed;
};

void FILstorageDevicePrivate::mountEntry()
{
      
#if defined (__APPLE__)
   struct statfs *mntbuf;
   int CountOfDevices;
   CountOfDevices = getmntinfo( &mntbuf, MNT_WAIT );
   
   // if an error occurs, 0 is returned by getmntinfo
   //
   if (CountOfDevices == 0)
   {
      COL_ERROR_STREAM_PLAIN("getmntinfo error", COLerror::PreCondition);
   }

   struct statfs *pFileSystemEntry;
   int i;

   for (pFileSystemEntry=mntbuf, i=0; i < CountOfDevices; pFileSystemEntry ++, i++)
   {
      if (strcmp(pFileSystemEntry->f_mntonname,DevicePath.c_str()) == 0)
      {
         Label = pFileSystemEntry->f_mntfromname; 
         FileSystemName = pFileSystemEntry->f_fstypename;
         SerialNumber = 0;
         MaxFileNameLength = 0;
         SpaceForMe = (COLuint64)pFileSystemEntry->f_bavail * pFileSystemEntry->f_bsize;
         SpaceForEveryOne = (COLuint64) pFileSystemEntry->f_bfree * pFileSystemEntry->f_bsize;
         SpaceUsed =  (COLuint64)(pFileSystemEntry->f_blocks - pFileSystemEntry->f_bfree) * pFileSystemEntry->f_bsize;
      }
   }

#elif defined (__linux__)
   FILE* FileHandle = setmntent("/etc/mtab", "r");
   if (NULL == FileHandle)
   {
      COL_ERROR_STREAM_PLAIN("setmntent failed to open /etc/mtab", COLerror::PreCondition);
   }
   // pray that no exceptions occur during the code below or it will need
   // to be changed
   struct mntent* pMountEntry=getmntent(FileHandle);
   while(pMountEntry != NULL)
   {
      if (strcmp(pMountEntry->mnt_dir, DevicePath.c_str())==0)
      {
         Label = pMountEntry->mnt_fsname; 
         FileSystemName = pMountEntry->mnt_type;

         // dumb check to make sure that we have a valid path
         if (DevicePath[0] != '/')
         {
            break;
         }

         struct statfs64 FileSystemInfo;

         if (statfs64(DevicePath.c_str(),&FileSystemInfo) != 0)
         {
            COL_THROW_STRERROR("statfs failed on path " << DevicePath << '.');
         }

         SerialNumber = 0;
         MaxFileNameLength = FileSystemInfo.f_namelen;
         SpaceForMe = FileSystemInfo.f_bavail * FileSystemInfo.f_bsize;
         SpaceForEveryOne = FileSystemInfo.f_bfree * FileSystemInfo.f_bsize;
         SpaceUsed =  (FileSystemInfo.f_blocks - FileSystemInfo.f_bfree) * FileSystemInfo.f_bsize;

         pMountEntry = NULL;
      }
      else
      { 
         pMountEntry = getmntent(FileHandle);
      }
   }
   // no error codes on endmntent - whoopee!
   endmntent(FileHandle);

#else
   COL_ERROR_STREAM_PLAIN("Platform not supported", COLerror::PreCondition);
#endif
}

FILstorageDevice::FILstorageDevice(const COLstring& DevicePath){
   COLassertIfMainThread();
   pMember = new FILstorageDevicePrivate;
   pMember->DevicePath = DevicePath;
   pMember->mountEntry();
}

FILstorageDevice::~FILstorageDevice()
{
   delete pMember;
}

const COLstring& FILstorageDevice::path() const
{
   return pMember->DevicePath;
}

const COLstring& FILstorageDevice::label() const
{
   return pMember->Label;
}

void FILstorageDevice::setLabel(const COLstring& Name)
{
}
 // What type of device is this
FILstorageDevice::FILstorageType FILstorageDevice::type() const
{
   return Unknown;
}
  
 // Name of the file system - i.e. FAT32, NTFS etc.
const COLstring& FILstorageDevice::fileSystemName() const
{
   return pMember->FileSystemName;
}

// Serial number of drive
COLuint32 FILstorageDevice::serialNumber() const
{
   return (COLuint32)pMember->SerialNumber;
}

COLuint32 FILstorageDevice::maximumFileNameLength() const
{
   return (COLuint32)pMember->MaxFileNameLength;
}

// returns the amount of space available to the user associated
// with the calling thread.
COLuint64 FILstorageDevice::spaceFreeForMe() const
{
   return pMember->SpaceForMe;
}

// returns the total amount of space available on the disk
COLuint64 FILstorageDevice::spaceFreeForEveryone() const
{
   return pMember->SpaceForEveryOne;
}

// returns the amount of data on the disk as seen by the user associated
// with the calling thread.
COLuint64 FILstorageDevice::spaceUsed() const
{
   return pMember->SpaceUsed;
}
