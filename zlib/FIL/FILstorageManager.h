#ifndef __FIL_STORAGE_MANAGER_H__
#define __FIL_STORAGE_MANAGER_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILstorageManager
//
// Description:
//
// This class together with FILstorageDevice is intended to provide
// an interface to the operating system calls for finding out the
// drives (mounts) available to an application, what type they are and
// how much storage space is available.
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class FILstorageManagerPrivate;
class COLstring;

class FILstorageManager
{
public:
   FILstorageManager();
   ~FILstorageManager();

   COLuint32 countOfDevice() const;
   const COLstring& deviceName(COLuint32 DeviceIndex) const;
private:
   FILstorageManagerPrivate* pMember;
   void setup();

   FILstorageManager(const FILstorageManager& Orig); // not allowed
   FILstorageManager& operator=(const FILstorageManager& Orig); // not allowed
};

#endif // end of defensive include
