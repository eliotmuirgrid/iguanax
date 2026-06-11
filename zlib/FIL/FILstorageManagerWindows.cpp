//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILstorageManager
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004 
//
//
//---------------------------------------------------------------------------
#include "FILprecomp.h"
#pragma hdrstop

#include "FILstorageManager.h"

#include <COL/COLerror.h>
#include <COL/COLarray.h>
#include <COL/COLioCheck.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

class FILstorageManagerPrivate
{
public:
   COLarray<COLstring> DeviceVector; 
};

FILstorageManager::FILstorageManager(){
   COL_METHOD(FILstorageManager::FILstorageManager);
   COLassertIfMainThread();
   pMember = new FILstorageManagerPrivate;
   try
   {
      setup();
   }
   catch(...)
   {
      delete pMember;
      throw;
   }
}

void FILstorageManager::setup()
{
   COLstring Buffer;
   COLuint32 RequiredLength = GetLogicalDriveStrings(Buffer.size(), Buffer.get_buffer());
   if (RequiredLength == 0)
   {
      COL_THROW_WIN32_ERROR("GetLogicalDriveStrings failed.");
   }
   Buffer.setCapacity(RequiredLength);
   Buffer.setSize(RequiredLength);
   RequiredLength = GetLogicalDriveStrings(Buffer.size(), Buffer.get_buffer());
   if (RequiredLength == 0  || RequiredLength > Buffer.size()){
      COL_THROW_WIN32_ERROR("GetLogicalDriveStrings failed.");
   }
   const char* CharPtr = (const char*)Buffer.data();
   while (*CharPtr != '\0') 
   {
      pMember->DeviceVector.push_back(CharPtr);
      while (*CharPtr != '\0')
      {
         CharPtr++;
      }
      CharPtr++;
   } 
}
 
FILstorageManager::~FILstorageManager()
{
   delete pMember;
}

COLuint32 FILstorageManager::countOfDevice() const
{
   return pMember->DeviceVector.size();   
}

const COLstring& FILstorageManager::deviceName(COLuint32 DeviceIndex) const
{
   return pMember->DeviceVector[DeviceIndex];  
}
