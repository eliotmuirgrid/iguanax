//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SIGslotCollectionVoid (copied from COLslotCollectionVoid.cpp).
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Thu Jan 20 15:04:10 EST 2005
//
//---------------------------------------------------------------------------

#include "SIGprecomp.h"
#pragma hdrstop

#include "SIGslotCollectionVoid.h"
#include "SIGsignallerVoid.h"

#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


class SIGslotCollectionVoidPrivate
{
public:
   SIGslotCollectionVoidPrivate(){
      COL_METHOD(SIGslotCollectionVoidPrivate::SIGslotCollectionVoidPrivate);
   }
   ~SIGslotCollectionVoidPrivate(){
      COL_METHOD(SIGslotCollectionVoidPrivate::~SIGslotCollectionVoidPrivate);
   }

   COLarray<void*> Slots;
   COLarray<void*> DeferredDeletionSlots;

private:
   SIGslotCollectionVoidPrivate(const SIGslotCollectionVoidPrivate&); // NO DICE
   SIGslotCollectionVoidPrivate& operator=(const SIGslotCollectionVoidPrivate&); // STOP IT

};

SIGslotCollectionVoid::SIGslotCollectionVoid() {
   COL_METHOD(SIGslotCollectionVoid::SIGslotCollectionVoid);
   pMember = new SIGslotCollectionVoidPrivate;
}

SIGslotCollectionVoid::~SIGslotCollectionVoid() {
   COL_METHOD(SIGslotCollectionVoid::~SIGslotCollectionVoid);
   delete pMember;
}

void* SIGslotCollectionVoid::slot(COLuint32 SlotIndex) {
   COL_METHOD(SIGslotCollectionVoid::slot);
   return pMember->Slots[SlotIndex];
}

COLuint32 SIGslotCollectionVoid::countOfSlotImp() const {
   COL_METHOD(SIGslotCollectionVoid::countOfSlotImp);
   COL_VAR2(pMember, &pMember->Slots);
   return pMember->Slots.size();
}

void SIGslotCollectionVoid::clearSlots( SIGsignallerVoid* pOwner ) {
   COL_METHOD(SIGslotCollectionVoid::clearSlots);
   for (COLuint32 SlotIndex = 0; SlotIndex < pMember->Slots.size(); SlotIndex++) {
      untrackSlot(pMember->Slots[SlotIndex], pOwner );
      destroySlot( pMember->Slots[SlotIndex], pOwner );
   }
   pMember->Slots.clear();
}

void SIGslotCollectionVoid::prepareForCall() {
   COL_METHOD(SIGslotCollectionVoid::prepareForCall);
   COLPRECONDITION(pMember->DeferredDeletionSlots.size() == 0)
   //this are also in progress...this lets us delete ourself safely
   setThisCallInProgress(true);
   //mark all slots that its in a call, allowing for deferred deletion
   markInProgress(true);
}

void SIGslotCollectionVoid::setCallComplete(SIGsignallerVoid* pOwner) {
   COL_METHOD(SIGslotCollectionVoid::setCallComplete);
   markInProgress(false);
   removeMarkedForDelete();
   setThisCallInProgress(false);
   checkAndDeleteSelf(pOwner);
}

void SIGslotCollectionVoid::doConnect(SIGsignallerVoid* pOwner, void* pNewSlot /*Must take ownership*/) {
   COL_METHOD(SIGslotCollectionVoid::doConnect);
   COLPRECONDITION( pNewSlot != NULL)
   COLPRECONDITION( !compareToNull(pNewSlot) )
   //find the slot if needed, if it exists, do nothing.
   COLuint32 SlotIndex = 0;
   while(SlotIndex < pMember->Slots.size()) {
      if ( compareSlots(pMember->Slots[SlotIndex],pNewSlot)) {
         break;
      } else {
         SlotIndex++;
      }
   }
   if (SlotIndex >= pMember->Slots.size()) {  //not found
      trackSlot(pNewSlot,pOwner);
      pMember->Slots.push_back(pNewSlot);
      pMember->Slots[pMember->Slots.size()-1];
   } else {
      destroySlot(pNewSlot, pOwner);
   }   
}

void SIGslotCollectionVoid::doDisconnect(SIGsignallerVoid* pOwner, void* pSlotToRemove) {
   COL_METHOD(SIGslotCollectionVoid::doDisconnect);
   COLPRECONDITION( !compareToNull(pSlotToRemove) );
   //find the slot, if it exists remove it.
   COLuint32 SlotIndex = 0;
   while(SlotIndex < pMember->Slots.size()) {
      if ( compareSlots(pMember->Slots[SlotIndex],pSlotToRemove)) {
         //see if it's currently in a call, if it is, move it to the deferred deletion list
         if (callInProgressVoid( pMember->Slots[SlotIndex] )) {
            COL_TRC("Slot has call in progress. Will untrack and set to null.");
            untrackSlot(pMember->Slots[SlotIndex], pOwner);
            pMember->DeferredDeletionSlots.push_back(pMember->Slots[SlotIndex]);
            //do not remove from the Slots vector, we are not allowed to adjust its size
            pMember->Slots[SlotIndex] = nullSlotInstance();
         } else {
            untrackSlot(pMember->Slots[SlotIndex], pOwner);
            destroySlot(pMember->Slots[SlotIndex], pOwner);
            pMember->Slots.remove(SlotIndex);
         }
         break;
      } else {
         SlotIndex++;
      }
   }
   checkAndDeleteSelf(pOwner);
}

void SIGslotCollectionVoid::doTrackerDestroy(SIGsignallerVoid* pOwner) {
   COL_METHOD(SIGslotCollectionVoid::doTrackerDestroy);
   //no need to hang on to the slots, just clear them now
   clearSlots(pOwner);
}

void SIGslotCollectionVoid::doTrackableDestroy(SIGsignallerVoid* pOwner, COLtrackable* pTrackable) {
   COL_METHOD(SIGslotCollectionVoid::doTrackableDestroy);
   //we don't call onTrackableDestroy on each Slot because we are assuming that
   //a SlotCollection can NEVER have other SlotCollections
   COLuint32 SlotIndex = 0;
   while(SlotIndex < pMember->Slots.size()) {
      //it is possible to get this call during a broadcast, defer if needed
      if ( compareTrackable(pMember->Slots[SlotIndex],pTrackable) ) {
         if (callInProgressVoid( pMember->Slots[SlotIndex] )) {
            COL_TRC("Slot has call in progress. Will set this index position to null.");
            untrackSlot(pMember->Slots[SlotIndex], pOwner);
            pMember->DeferredDeletionSlots.push_back(pMember->Slots[SlotIndex]);
            //do not remove from the Slots vector, we are not allowed to adjust its size
            pMember->Slots[SlotIndex] = nullSlotInstance();
         } else {
            untrackSlot(pMember->Slots[SlotIndex], pOwner);
            destroySlot(pMember->Slots[SlotIndex], pOwner);
            pMember->Slots.remove(SlotIndex);
         }
      } else {
         SlotIndex++;
      }
   }
   if (pOwner) {
      checkAndDeleteSelf(pOwner); 
   }
}

void SIGslotCollectionVoid::markInProgress(bool InProgress) {
   COL_METHOD(SIGslotCollectionVoid::markInProgress);
   for (COLuint32 SlotIndex = 0; SlotIndex < pMember->Slots.size(); SlotIndex++) {
      //pMember->Slots[SlotIndex] = 0;
      setCallInProgressVoid(pMember->Slots[SlotIndex],InProgress);
   }
}

void SIGslotCollectionVoid::checkAndDeleteSelf(SIGsignallerVoid* pOwner) {
   COL_METHOD(SIGslotCollectionVoid::checkAndDeleteSelf);
   if ( thisCallInProgress() ) {
      COL_TRC("Call in progress. Doing nothing.");
      return;
   } 
   if (pMember->Slots.size() == 0) {
      //if the size is now zero, we must switch ourself back to null slot mode
      pOwner->pVoidSlotPrivate = nullSlotInstance();
      clearSlots(pOwner);
      delete this;
   } else if (pMember->Slots.size() == 1) {
      //move back to 1 mode
      pOwner->pVoidSlotPrivate = pMember->Slots[0];
      //the remove is important, because we are transferring ownership
      pMember->Slots.remove(0);
      delete this;
   }
}

void SIGslotCollectionVoid::removeMarkedForDelete() {
   COL_METHOD(SIGslotCollectionVoid::removeMarkedForDelete);
   COL_VAR2(pMember, &pMember->DeferredDeletionSlots);
   COL_VAR(pMember->DeferredDeletionSlots.size());
   for (COLuint32 SlotIndex = 0; SlotIndex < pMember->DeferredDeletionSlots.size(); SlotIndex++) {         
      //no longer tracked
      COL_VAR(SlotIndex);
      if (pMember->DeferredDeletionSlots[SlotIndex] == NULL) {
         COL_TRC("THIS IS SO HOSED");
      } else {
         destroySlot(pMember->DeferredDeletionSlots[SlotIndex], NULL );
      }
   }
   if (pMember->DeferredDeletionSlots.size()) {
      //must check and remove the null entries in the Slots list
      COLuint32 SlotIndex = 0;
      while( SlotIndex < pMember->Slots.size()) {
         if (pMember->Slots[SlotIndex] == nullSlotInstance()) {
            pMember->Slots.remove(SlotIndex);
         } else {
            SlotIndex++;
         }
      }
      pMember->DeferredDeletionSlots.clear();
   }
}
