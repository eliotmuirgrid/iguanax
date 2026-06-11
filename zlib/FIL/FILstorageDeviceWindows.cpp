//--------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
#include "FILprecomp.h"
#pragma hdrstop

#include "FILstorageDevice.h"

#include <COL/COLerror.h>
#include <COL/COLioCheck.h>

class FILstorageDevicePrivate
{
public:
   COLstring DevicePath;
   COLstring Label;
   DWORD SerialNumber;
   DWORD MaxFileNameLength;
   DWORD SystemFlags;
   COLstring FileSystemName;

   ULARGE_INTEGER SpaceForMe;
   ULARGE_INTEGER SpaceForEveryOne;
   ULARGE_INTEGER SpaceUsed;
};


FILstorageDevice::FILstorageDevice(const COLstring& DevicePath){
   COLassertIfMainThread();
   try {
      pMember = new FILstorageDevicePrivate;
      pMember->DevicePath = DevicePath;

      // TODO - SetErrorMode(SEM_FAILCRITICALERRORS); ?
      pMember->Label.assign(256, '\0');
      pMember->FileSystemName.assign(256, '\0');

      COL_CHECK_WIN32_CALL(GetVolumeInformation(DevicePath.c_str(), 
                              pMember->Label.get_buffer(), 
                              pMember->Label.size(),
                              &pMember->SerialNumber,
                              &pMember->MaxFileNameLength,
                              &pMember->SystemFlags,
                              pMember->FileSystemName.get_buffer(),
                              pMember->FileSystemName.size()));

      ULARGE_INTEGER TotalSpace;
      pMember->Label = pMember->Label.substr(0, strlen(pMember->Label.c_str()));
   
      COL_CHECK_WIN32_CALL(GetDiskFreeSpaceEx(DevicePath.c_str(),
                              &pMember->SpaceForMe,
                              &TotalSpace,
                              &pMember->SpaceForEveryOne));   

      pMember->SpaceUsed.QuadPart = TotalSpace.QuadPart - (pMember->SpaceForEveryOne.QuadPart);
   }
   catch(...)
   {
      if(pMember)
      {
         delete pMember;
         pMember = NULL;
      }
      throw;
   }
   
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
   COL_CHECK_WIN32_CALL(SetVolumeLabel(pMember->DevicePath.c_str(), Name.c_str()));
}

 // What type of device is this
FILstorageDevice::FILstorageType FILstorageDevice::type() const
{
   switch(GetDriveType(pMember->DevicePath.c_str()))
   {
   case DRIVE_UNKNOWN:
      return Unknown;
   case DRIVE_NO_ROOT_DIR:
      COL_ERROR_STREAM_PLAIN(pMember->DevicePath.c_str() << " does not exist", COLerror::PreCondition);
      break;
   case DRIVE_REMOVABLE:
      return Removable;
   case DRIVE_FIXED:
      return Fixed;
   case DRIVE_REMOTE:
      return Network;
   case DRIVE_CDROM:
      return CdRom;
   case DRIVE_RAMDISK:
      return RamDisc;
   default:
      return Unknown;
   }
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
   return pMember->SpaceForMe.QuadPart;
}

// returns the total amount of space available on the disk
COLuint64 FILstorageDevice::spaceFreeForEveryone() const
{
   return pMember->SpaceForEveryOne.QuadPart;
}

// returns the amount of data on the disk as seen by the user associated
// with the calling thread.
COLuint64 FILstorageDevice::spaceUsed() const
{
   return pMember->SpaceUsed.QuadPart;
}
