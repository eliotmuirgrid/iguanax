//---------------------------------------------------------------------------
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SIGsignallerMheader (copied from COLsignalMheader.h).
//
// Author: Nasron Cheong
//
// Description:
//
// This header is included as required by COlsignal1.h, SIGsignaller2.h ...etc
//
//
//---------------------------------------------------------------------------
#include <COL/COLerror.h>

#include "SIGsignallerVoid.h"

//SIGNAL
template < SIG_SIGNALLER_TEMPLATE_PARAMS >
class SIG_SIGNALLER_TEMPLATE_NAME(SIGsignaller) : 
   public SIGsignallerVoid
{
private:
   typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)<SIG_SIGNALLER_PARAMS_IMPL> SlotBaseType;
   typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)<SIG_SIGNALLER_PARAMS_IMPL> SlotNullType;
   typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCallback)<SIG_SIGNALLER_PARAMS_IMPL> SlotFunctionType;
   typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCollection)<SIG_SIGNALLER_PARAMS_IMPL> SlotCollectionType;
   
public:
   SIG_SIGNALLER_TEMPLATE_NAME(SIGsignaller)()
   {
      pVoidSlotPrivate = &SlotNullType::instance();
   }
   virtual ~SIG_SIGNALLER_TEMPLATE_NAME(SIGsignaller)()
   {
      ((SlotBaseType*)this->pVoidSlotPrivate)->onTrackerDestroy(this);
      if ( this->pVoidSlotPrivate != &SlotNullType::instance() )
      {
         delete (SlotBaseType*)this->pVoidSlotPrivate;
      }
   }

   // Find out how many slots we have connected to this SIGsignaller object.
   COLuint32 countOfSlot() const
   {
      return ((SlotBaseType*)pVoidSlotPrivate)->countOfSlot();
   }

//FUNCTION ADD/REMOVE RECEIVER
   void addReceiver( typename SlotFunctionType::TCallbackFunction pFunction)
   {
      SlotFunctionType* pNewSlot = new SlotFunctionType(pFunction);
      ((SlotBaseType*)this->pVoidSlotPrivate)->connect(this,pNewSlot);
   }

   void removeReceiver( typename SlotFunctionType::TCallbackFunction pFunction)
   {
      //changing slot, notify slot's instance that we no longer care.
      SlotFunctionType NewSlot(pFunction);
      ((SlotBaseType*)this->pVoidSlotPrivate)->disconnect(this,NewSlot);
   }
   
   void clear()
   {
      // we are not really destroying ourselves, but we need to notify the COLtrackable's now
      // anyway.
      ((SlotBaseType*)this->pVoidSlotPrivate)->onTrackerDestroy(this); 
      if ( this->pVoidSlotPrivate != &SlotNullType::instance() )
      {
         delete ((SlotBaseType*)this->pVoidSlotPrivate);
         pVoidSlotPrivate = &SlotNullType::instance();
      }
   }

//METHOD ADD/REMOVE RECEIVER
   template < class _Class >
   void addReceiver( void (_Class::*pMethod)(SIG_SIGNALLER_PARAMS) ,_Class* pInstance)
   {
      if (!pInstance)
      {
         COL_ERROR_STREAM("Class instance is null",COLerror::PreCondition);
      }

      typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotVoidMethod)< SIG_SIGNALLER_PARAMS_IMPL_WITH_CLASS > SlotMethodType;
      SlotMethodType* pNewSlot = new SlotMethodType(pInstance,pMethod);
      ((SlotBaseType*)this->pVoidSlotPrivate)->connect(this,pNewSlot);
   }
      
   template < class _Class >
   void removeReceiver( void (_Class::*pMethod)(SIG_SIGNALLER_PARAMS) ,_Class* pInstance)
   {
      if (!pInstance)
      {
         COL_ERROR_STREAM("Class instance is null",COLerror::PreCondition);
      }
      typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotVoidMethod)< SIG_SIGNALLER_PARAMS_IMPL_WITH_CLASS > SlotMethodType;
      SlotMethodType NewSlot(pInstance,pMethod);
      ((SlotBaseType*)this->pVoidSlotPrivate)->disconnect(this,NewSlot);
   }
   
   void signal( SIG_SIGNALLER_PARAMS_NAMES )
   {
      (*((SlotBaseType*)this->pVoidSlotPrivate))( SIG_SIGNALLER_PARAMS_NAMES2_WITH_OWNER(this) );
   }

   virtual void onTrackableDestroy(COLtrackable* pTrackable)
   {
      ((SlotBaseType*)this->pVoidSlotPrivate)->onTrackableDestroy(this,pTrackable);
   }

   virtual bool operator==( const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL >& Orig)
   {
      return false;
   }

private:
   SIG_SIGNALLER_TEMPLATE_NAME(SIGsignaller)& operator=(const SIG_SIGNALLER_TEMPLATE_NAME(SIGsignaller)&);
   SIG_SIGNALLER_TEMPLATE_NAME(SIGsignaller)(const SIG_SIGNALLER_TEMPLATE_NAME(SIGsignaller)&);
};
