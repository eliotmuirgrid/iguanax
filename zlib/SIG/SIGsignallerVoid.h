#ifndef __SIG_SIGNALLER_VOID_H__
#define __SIG_SIGNALLER_VOID_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SIGsignallerVoid (copied from COLsignalVoid.h).
//
// Author: Nasron Cheong
//
// Description: This class is used to pass around the signal to its slots
// without always declaring a new template type.
//
// Aslo needed by SIGslotCollectionVoid
//
//
//---------------------------------------------------------------------------

#include <COL/COLtracker.h>

class SIGsignallerVoid : public COLtracker
{
public:
   SIGsignallerVoid() : pVoidSlotPrivate(NULL)
   {
   }
   virtual ~SIGsignallerVoid()
   {
      //Note that the cleanup is deferred to the derived destructor
   }

   //this is not made private, because its difficult to get all the 
   //friend declarations right.
   void* pVoidSlotPrivate;

private:
   SIGsignallerVoid& operator=(const SIGsignallerVoid&);
   SIGsignallerVoid(const SIGsignallerVoid&);
};

#endif // end of defensive include
