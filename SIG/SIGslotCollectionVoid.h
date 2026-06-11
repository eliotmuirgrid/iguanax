#ifndef __SIG_SLOT_COLLECTION_VOID_H__
#define __SIG_SLOT_COLLECTION_VOID_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SIGslotCollectionVoid (copied from COLslotCollectionVoid.h).
//
// Description:
//
// This class is used to template hoist the SIGslotCollectionN template
// See SIGslotCollectionN
//
// Author: Nasron Cheong
// Date:   Thu Jan 20 15:04:10 EST 2005
//
//---------------------------------------------------------------------------
#include <COL/COLminimumInclude.h>

class SIGslotCollectionVoidPrivate;
class SIGsignallerVoid;
class COLtrackable;

class SIGslotCollectionVoid
{
public:
   SIGslotCollectionVoid();
   virtual ~SIGslotCollectionVoid();

protected:

   //
   // Removes all contained slots and destroys them
   //
   void clearSlots(SIGsignallerVoid* pOwner);

   //
   // Marks all slots that if a disconnect is done on this,
   // deferred deletion can be done.
   //
   void prepareForCall();

   //
   // Unmark all slots, and delete slots that were deferred for deletion
   //
   void setCallComplete(SIGsignallerVoid* pOwner);

   //
   // Handle the connection operation. Basically, if an equivalent slot doesn't
   // already exist, add it, else, delete the slot argument.
   //
   void doConnect(SIGsignallerVoid* pOwner, 
                  void* pNewSlot /*Must take ownership*/);

   //
   // Handle the disconnect operation.
   // Depending on if the pSlotToRemove is InProgress, either defer its deletion,
   // or delete it immediately.
   //
   void doDisconnect(SIGsignallerVoid* pOwner, 
                     void* pSlotToRemove);

   //
   // Handle the tracker destroy method,
   // which broadcasts to all slots that have trankable instances, that this tracker
   // is being destroyed, which allows them to unlisten for events.
   //
   void doTrackerDestroy(SIGsignallerVoid* pOwner);

   //
   // Handle the trackable destroy method.
   // This will check all slots for trackable instances, and remove them or defer delete
   // thme depending on if they are currently being called or not
   //
   void doTrackableDestroy(SIGsignallerVoid* pOwner,
                           COLtrackable* pTrackable);

   //
   // Removes slots that were deferred for deletion
   //
   void removeMarkedForDelete();

   //
   // Accessor methods to allow the derived template to do its function
   // call broadcast
   //
   void* slot(COLuint32 SlotIndex);
   COLuint32 countOfSlotImp() const;

//
// The following virtual functions that be implemented by the templated class
//

   //
   // call the setInProgress on the derived class
   //
   virtual void setThisCallInProgress(bool InProgress) = 0;

   //
   // return the value of the derived class being in progress
   //
   virtual bool thisCallInProgress() = 0;
   
   //
   // calls setInProgress on the given slot
   //
   virtual void setCallInProgressVoid(void* pSlot,bool InProgress) = 0;

   //
   // returns callInProgress on the given slot
   //
   virtual bool callInProgressVoid(void* pSlot) = 0;

   //
   // return the result of the == operation on both slots
   //
   virtual bool compareSlots(void* pSlot1, void* pSlot2) = 0;

   //
   // return if the slot is the SIGslotNullN instance
   //
   virtual bool compareToNull(void* pSlot) = 0;

   //
   // return the SIGslotNullN instance
   //
   virtual void* nullSlotInstance() = 0;

   //
   // do a properly cast deletion of the given slot
   //
   virtual void destroySlot(void* pSlot,SIGsignallerVoid* pOwner) = 0;

   //
   // if the slot has a trackable instance, track it with the given SIGsignaller
   //
   virtual void trackSlot(void* pSlot,SIGsignallerVoid* pOwner) = 0;

   //
   // if the slot has a trackable instance, untrack it with the given SIGsignaller
   //
   virtual void untrackSlot(void* pSlot,SIGsignallerVoid* pOwner) = 0;

   //
   // if the slot has a trackable instance, return if the given trackable instance 
   // is the same instance
   //
   virtual bool compareTrackable(void* pSlot, COLtrackable* pTrackable) = 0;

private:

   //
   // Mark all contained slots that they are in progress
   //
   void markInProgress(bool InProgress);

   //
   // Move back to either one or null mode depending on the remaining number of slots
   //
   void checkAndDeleteSelf(SIGsignallerVoid* pOwner);
   
   SIGslotCollectionVoidPrivate* pMember;
   SIGslotCollectionVoid(const SIGslotCollectionVoid&); 
   SIGslotCollectionVoid& operator=(const SIGslotCollectionVoid&);
};

#endif // end of defensive include
