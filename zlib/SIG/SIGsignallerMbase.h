//---------------------------------------------------------------------------
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SIGsignallerMbase (copied from COLsignalMbase.h).
//
//
// Description: This file CANNOT be included manually, is has no defensive includes
// for good reason, its supposed to be used by COlsignal1.h, SIGsignaller2.h ...etc
//
// The following is the hierachy of the slots, which is important to understand to
// maintain the library. Let N be the parameter count
//
// Slot Hierachy:
//
// SIGvoidMethodN (No derived Classes)
//
// SIGslotBaseN
//    - SIGslotNullN
//    - SIGslotCollectionN
//    - SIGslotSingleBaseN
//
// SIGslotSingleBaseN
//    - SIGslotVoidMethodN
//    - SIGslotCallbackN
//
// SIGslotCollectionVoid
//    - SIGslotCollectionN
//
//
// Author: Nasron Cheong
// Date  : Feb 2005
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// SIGvoidMethodN
//
// The class is no longer related to SIGsignaller, but is still used on its own in
// some code. It is similiar to SIGcallback, but does binding on the instance
// in addition to the function pointer
//---------------------------------------------------------------------------
template <class _Class, SIG_SIGNALLER_TEMPLATE_PARAMS >
class SIG_SIGNALLER_TEMPLATE_NAME(SIGvoidMethod)
{
public:

   typedef void (_Class::*TMethod)(SIG_SIGNALLER_PARAMS);

   SIG_SIGNALLER_TEMPLATE_NAME(SIGvoidMethod)(_Class* ipInstance,TMethod ipMethod) : pMethod(ipMethod), pInstance(ipInstance)
   {
   };
   SIG_SIGNALLER_TEMPLATE_NAME(SIGvoidMethod)()
    : pMethod(NULL),
      pInstance(NULL)
   {
   };
   virtual ~SIG_SIGNALLER_TEMPLATE_NAME(SIGvoidMethod)(){};

   SIG_SIGNALLER_TEMPLATE_NAME(SIGvoidMethod)& operator=(TMethod OrigMethod)
   {
      pMethod = OrigMethod;
      return *this;
   }

   SIG_SIGNALLER_TEMPLATE_NAME(SIGvoidMethod)& operator=(_Class* ipInstance)
   {
      pInstance = ipInstance;
      return *this;
   }

   _Class* classInstance()
   {
      return pInstance;
   }

   virtual void operator ()( SIG_SIGNALLER_PARAMS_NAMES )
   {
      if (pInstance && pMethod)
      {
         (pInstance->*pMethod)( SIG_SIGNALLER_PARAMS_NAMES2 );
      }
   }

protected:
   TMethod pMethod;
   _Class* pInstance;
};

//---------------------------------------------------------------------------
// SIGslotBaseN
//
// This base class allows for 2 things:
// 1) SIGsignaller's can have any slot implementation pointed to them
// 2) Polymorphic calls on the slot allow calls from SIGsignaller to change slots
//    on the fly, which means that the most appropriate slot implementation
//    can be selected, as disconnects and connects are done. This is a mixture
//    of a Composite and State pattern.
//
// The State transitions are:
//
// 1) SIGsignallers start by pointing to the SIGslotNull static instance
// 2) If a connect is done, it repoints the SIGsignaller to one of the SIGslotSingleBases
//    (SIGslotVoidMethod) through the SIGslotNull::connect call
// 3) If another connect is done, it repoints to the SIGslotCollection, and adds
//    the current slot to that new collection through SIGslotSingleBase::connect
// 4) If a disconnect is done, the SIGslotCollection will repoint the COlsignal
//    to remaining the SIGslotSingleBase thats in the SIGslotCollection.
// 5) If a disconnect is done again, it repoints the SIGsignaller to the SIGnullSlotInstance
//    and now we go back to 1)
//
//---------------------------------------------------------------------------
template < SIG_SIGNALLER_TEMPLATE_PARAMS >
class SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)
{
private:
   typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL > SlotBaseType;
   typedef SIGsignallerVoid SignalType;

public:
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)()  : CallInProgress(false) {}
   virtual ~SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)() {}

   // Returns the number of connected slots - useful for finding out if a signal
   // has been pointed to any slots
   virtual COLuint32 countOfSlot() const=0;

   //
   // Implementations should use this function to invoke the function call,
   // this is called from the signal
   //
   virtual void operator() ( SIG_SIGNALLER_PARAMS_NAMES_WITH_OWNER ) = 0;

   //
   // A static instance used to represent the type of the SIGslotBase
   // This is a simple run time type check on a fixed hierachy,
   // which makes the operator ==  work
   //
   virtual const SlotBaseType* typeInstance() const = 0;

   //
   // Implementations return true if the Rhs is the exact same type (check using
   // typeInstance(), therefore allowing a safe up-cast, and that the members are
   // the same
   //
   virtual bool operator==( const SlotBaseType& Orig) = 0;

   //
   // SIGslotCollections need to mark SIGslotBase's before invoking,
   // so it knows not to defer the deletion of slots while doing a function call
   // This is the flag for that purpose.
   //
   void setCallInProgress(bool In)
   {
      CallInProgress = In;
   }

   //
   // Indicates if the call is in progress
   // Usually only set by SIGslotCollection
   //
   bool callInProgress()
   {
      return CallInProgress;
   }

   //
   // Returns a trackable instance held by the slot, needed to know if
   // the instance should be notified that the tracker is going out of scope.
   //
   // Returns NULL if no such instance, such as in a SIGslotCollection, or SIGslotCallback
   //
   virtual COLtrackable* trackedInstance() = 0;

   //
   // SIGsignaller defers connection operations to its current slot
   // The following functions implement the state pattern

   //
   // Should only need to be implemented by SIGslotCollection,
   // SIGslotSingleBase and SIGslotNull
   //
   // Basically, replaces the signal's slot pointer during a connect/disconnect
   // and does some memory cleanup
   //
   virtual void connect(SignalType* pOwner,
                        SlotBaseType* pNewSlot /*Must take ownership*/) = 0;
   virtual void disconnect(SignalType* pOwner,
                           SlotBaseType& SlotToRemove) = 0;


   //
   // Allows deferring of trackable events to the slots so they can properly handle them.
   //
   virtual void onTrackerDestroy(SignalType* pOwner) = 0;
   virtual void onTrackableDestroy(SignalType* pOwner,
                                   COLtrackable* pTrackable) = 0;

private:
   bool CallInProgress;
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)& operator=(const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)&);
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)(const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)&);
};

//---------------------------------------------------------------------------
// SIGslotNull
//
// This class moves to SIGslotSingleBase when a connect is done, and does nothing
// when a disconnect is done.
//
// It is a singleton, because there is no need to have multiple instances of a class
// that has no state between different instances....saves memory and lets you do some
// pointer compares.
//---------------------------------------------------------------------------
template < SIG_SIGNALLER_TEMPLATE_PARAMS >
class SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull) :
     public SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL >
{
private:
   typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL > SlotBaseType;
   typedef SIGsignallerVoid SignalType;

   //
   // Enforce Singleton, privatize constructor
   //
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)()
   {
   };

public:
   virtual ~SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)() {}

   // Zero slots connected.
   virtual COLuint32 countOfSlot() const
   {
      return 0;
   }
   //
   // No such instance
   //
   virtual COLtrackable* trackedInstance(){ return NULL; }

   //
   // This allows signals to make a blank slot call without an if statement
   //
   virtual void operator()( SIG_SIGNALLER_PARAMS_NAMES_WITH_OWNER )
   {
      //do nothing
   }

   //
   // Basically never true unless Rhs is itself
   //
   virtual bool operator==( const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL >& Orig)
   {
      if (Orig.typeInstance() == typeInstance() && &Orig == this) //check type and singleton-nes
      {
         return true;
      }
      return false;
   }


   //
   // Move to single slot mode
   //
   virtual void connect(SignalType* pOwner,
                        SlotBaseType* pNewSlot /*Must take ownership*/)
   {
      //switch to single slot
      COLPRECONDITION(pNewSlot != NULL)
      COLPRECONDITION(pOwner->pVoidSlotPrivate == this)
      pOwner->pVoidSlotPrivate = pNewSlot;
      if (pNewSlot->trackedInstance())
      {
         pNewSlot->trackedInstance()->addTracker(pOwner);
      }
      //this is a singleton, do not delete this
   }

   //
   // Nothing to disconnect, so nothing to do
   //
   virtual void disconnect(SignalType* pOwner,
                           SlotBaseType& SlotToRemove)
   {
      //nothing to do
      COLPRECONDITION(pOwner->pVoidSlotPrivate == this)
   }

   //
   // Nothing to do since there is no trackable instance
   //
   virtual void onTrackerDestroy(SignalType* pOwner){};
   virtual void onTrackableDestroy(SignalType* pOwner, COLtrackable* pTrackable){};

   virtual const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL >* typeInstance() const;
   static SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)& instance();

private:
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)& operator=(const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)&);
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)(const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)&);
};

//
// These functions have to be declared here to properly instantiate the static instances
// since it requires its own type information before an instance can be made.
//
template< SIG_SIGNALLER_TEMPLATE_PARAMS_IMPL >
const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL >* SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)< SIG_SIGNALLER_PARAMS_IMPL >::typeInstance() const
{
   static SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)< SIG_SIGNALLER_PARAMS_IMPL > TypeInstance;
   return &TypeInstance;
}

template< SIG_SIGNALLER_TEMPLATE_PARAMS_IMPL >
SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)< SIG_SIGNALLER_PARAMS_IMPL >& SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)< SIG_SIGNALLER_PARAMS_IMPL >::instance()
{
   static SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)< SIG_SIGNALLER_PARAMS_IMPL > Instance;
   return Instance;
}

//---------------------------------------------------------------------------
// SIGslotCollection
//
// This slot contains other slots EXCEPT for other SIGslotCollections, because we do
// not need a tree structure...not yet anyway.
//
// A thin template implementation is here, see SIGslotCollectionVoid to see the majority of
// the code that actually does the work.
// Most of the code here is to provide type information to the base void class
//---------------------------------------------------------------------------
template < SIG_SIGNALLER_TEMPLATE_PARAMS >
class SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCollection) :
     public SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL >,
     public SIGslotCollectionVoid
{
private:
   typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL > SlotBaseType;
   typedef SIGsignallerVoid SignalType;
   typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)< SIG_SIGNALLER_PARAMS_IMPL > SlotNullType;

public:
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCollection)()
   {
   };
   virtual ~SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCollection)()
   {
      // remove any slots that were deferred for deletion
      // we don't untrack because items in the deferred list are already untracked
      removeMarkedForDelete();
      // we should not have any remaining slots, they should have been removed before deletion,
      // to ensure things are properly untracked.
      // Note: C++11 defaults destructors to "noexcept(true)". App will crash if another
      // exception is already propagating. If we really want to crash here, append
      // "noexcept(false)" to function signature and uncomment COLPOSTCONDITION below.
      // COLPOSTCONDITION( countOfSlotImp() == 0);
   };

   // Notice the use of countOfSlotImp?  This is the non-virtual method that the parent
   // class SIGsignallerVoid implements.  I have deliberately avoided making that method virtual
   // since this would have some (small) performance implications for SIGslotCollection.
   virtual COLuint32 countOfSlot() const
   {
      return countOfSlotImp();
   }
   //
   // No such instance associated with this slot.
   // The containing slots may or may not have such instances
   //
   virtual COLtrackable* trackedInstance(){ return NULL; }

   //
   // The function basically prepares itself by marking all contained slots
   // as InProgress, so that if a disconnect is done during this function call
   // on a slot on this collection, we can defer the deletion to the
   // setCallComplete method
   //
   virtual void operator()( SIG_SIGNALLER_PARAMS_NAMES_WITH_OWNER )
   {
      prepareForCall();
      for (COLuint32 Index = 0; Index < countOfSlotImp(); Index++)
      {
         try
         {
            //Note that we assume that the slots are NEVER other SIGslotCollections
            (*((SlotBaseType*)slot(Index)))( SIG_SIGNALLER_PARAMS_NAMES2_WITH_OWNER(pOwner) );
         }
         catch(...)
         {
            setCallComplete(pOwner);
            throw;
         }
      }
      setCallComplete(pOwner);
   }

   //
   // We always return false....most of the time the == operator is used only on non-collection
   // SIGslotBases
   //
   virtual bool operator==( const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL >& Orig)
   {
      return false;
   }

   //
   // The instance representing this type
   //
   virtual const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL >* typeInstance() const
   {
      static SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCollection)< SIG_SIGNALLER_PARAMS_IMPL > TypeInstance;
      return &TypeInstance;
   }

   //
   // See SIGslotCollectionVoid
   //
   virtual void connect(SignalType* pOwner,
                        SlotBaseType* pNewSlot /*Must take ownership*/)
   {
      doConnect(pOwner,pNewSlot);
   }

   //
   // See SIGslotCollectionVoid
   //
   virtual void disconnect(SignalType* pOwner,
                           SlotBaseType& SlotToRemove)
   {
      doDisconnect(pOwner,&SlotToRemove);
   }

   //
   // See SIGslotCollectionVoid
   //
   virtual void onTrackerDestroy(SignalType* pOwner)
   {
      doTrackerDestroy(pOwner);
   };

   //
   // See SIGslotCollectionVoid
   //
   virtual void onTrackableDestroy(SignalType* pOwner,
                                   COLtrackable* pTrackable)
   {
      doTrackableDestroy(pOwner,pTrackable);
   }
protected:

   //
   // These methods provide operations requiring type information
   // See SIGslotCollectionVoid
   //
   virtual void setThisCallInProgress(bool InProgress)
   {
      this->setCallInProgress(InProgress);
   }

   virtual bool thisCallInProgress()
   {
      return this->callInProgress();
   }

   virtual void setCallInProgressVoid(void* pSlot,bool InProgress)
   {
      ((SlotBaseType*)pSlot)->setCallInProgress(InProgress);
   }

   virtual bool callInProgressVoid(void* pSlot)
   {
      return ((SlotBaseType*)pSlot)->callInProgress();
   }

   virtual bool compareSlots(void* pSlot1, void* pSlot2)
   {
      return (*((SlotBaseType*)pSlot1)) == (*((SlotBaseType*)pSlot2));
   }

   virtual bool compareToNull(void* pSlot)
   {
      return ((SlotBaseType*)pSlot) == &SlotNullType::instance();
   }

   virtual void* nullSlotInstance()
   {
      return &SlotNullType::instance();
   }

   virtual void destroySlot(void* pSlot,SIGsignallerVoid* pOwner)
   {
      delete (SlotBaseType*)pSlot;
   }

   virtual void trackSlot(void* pSlot,SIGsignallerVoid* pOwner)
   {
      if (((SlotBaseType*)pSlot)->trackedInstance())
      {
         ((SlotBaseType*)pSlot)->trackedInstance()->addTracker(pOwner);
      }
   }

   virtual void untrackSlot(void* pSlot,SIGsignallerVoid* pOwner)
   {
      if (((SlotBaseType*)pSlot)->trackedInstance())
      {
         ((SlotBaseType*)pSlot)->trackedInstance()->removeTracker(pOwner);
      }
   }

   virtual bool compareTrackable(void* pSlot, COLtrackable* pTrackable)
   {
      return ((SlotBaseType*)pSlot)->trackedInstance() == pTrackable;
   }

private:
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCollection)& operator=(const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCollection)&);
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCollection)(const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCollection)&);
};

//---------------------------------------------------------------------------
// SIGslotSingleBase
//
// This is a base class for all slots that are of a single type
// allows the connect and disconnect operation to be shared
//
//---------------------------------------------------------------------------
template < SIG_SIGNALLER_TEMPLATE_PARAMS >
class SIG_SIGNALLER_TEMPLATE_NAME(SIGslotSingleBase) :
     public SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL >
{
private:
   typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL > SlotBaseType;
   typedef SIGsignallerVoid SignalType;
public:
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotSingleBase)()
   {
   };
   virtual ~SIG_SIGNALLER_TEMPLATE_NAME(SIGslotSingleBase)()
   {};

   virtual COLuint32 countOfSlot() const
   {
      return 1;
   }
   //
   // Move to the SIGslotCollection state and adds itself to the collection
   // basically transferring ownership
   //
   virtual void connect(SignalType* pOwner,
                        SlotBaseType* pNewSlot /*Must take ownership*/)
   {
      //decide if we need to switch to multi slot, or remain in the current slot type
      COLPRECONDITION(pNewSlot != NULL)
      if (!(*pNewSlot == *this))
      {
         COLPRECONDITION(pOwner->pVoidSlotPrivate == this)
         pOwner->pVoidSlotPrivate = new SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCollection)< SIG_SIGNALLER_PARAMS_IMPL >;

         //defer the call
         ((SlotBaseType*)(pOwner->pVoidSlotPrivate))->connect(pOwner,pNewSlot);

         //defer again for ourself, because this slot needs to be included in the broadcast
         ((SlotBaseType*)(pOwner->pVoidSlotPrivate))->connect(pOwner,this);
      }
      else
      {
         //else we point to the same slot essentially, theres nothing to do
         delete pNewSlot;
      }
   }

   //
   // Move back to the SIGslotNull state if needed
   //
   virtual void disconnect(SignalType* pOwner,
                           SlotBaseType& SlotToRemove)
   {
      //we should be only disconnecting if its the same slot
      if (*this == SlotToRemove)
      {
         if (this->trackedInstance())
         {
            this->trackedInstance()->removeTracker(pOwner);
         }
         COLPRECONDITION(pOwner->pVoidSlotPrivate == this)
         delete this;
         pOwner->pVoidSlotPrivate = &SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)< SIG_SIGNALLER_PARAMS_IMPL >::instance();
      }
   }

private:
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotSingleBase)& operator=(const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotSingleBase)&);
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotSingleBase)(const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotSingleBase)&);
};

//---------------------------------------------------------------------------
// SIGslotCallback
//
// A slot representing a function call to a plain function
//
//---------------------------------------------------------------------------
template < SIG_SIGNALLER_TEMPLATE_PARAMS >
class SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCallback) :
     public SIG_SIGNALLER_TEMPLATE_NAME(SIGslotSingleBase)< SIG_SIGNALLER_PARAMS_IMPL >
{
private:
   typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL > SlotBaseType;
   typedef SIGsignallerVoid SignalType;

public:
   typedef void (COL_STD_CALL *TCallbackFunction)( SIG_SIGNALLER_PARAMS );

   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCallback)(TCallbackFunction ipFunction)
      : pFunction(ipFunction)
   {
   };
   virtual ~SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCallback)()
   {};

   //
   // No such instance
   //
   virtual COLtrackable* trackedInstance(){ return NULL; }

   //
   // Just call the function. We do not do a precondition check to avoid the runtime cost
   //
   virtual void operator()( SIG_SIGNALLER_PARAMS_NAMES_WITH_OWNER )
   {
      //note that we don't have to worry about callInProgress here, because if we do delete ourself,
      //we don't do anything after the function call....safe.
      (*pFunction)( SIG_SIGNALLER_PARAMS_NAMES2 );
   }

   //
   // returns true if Rhs is the same type and points to the same function
   //
   virtual bool operator==( const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL >& Orig)
   {
      if (Orig.typeInstance() == typeInstance())
      {  //safe to cast
         typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCallback)< SIG_SIGNALLER_PARAMS_IMPL > TThis;
         const TThis& Cast = (const TThis&)Orig;
         if (Cast.pFunction == this->pFunction)
         {
            return true;
         }
      }
      return false;
   }

   //
   // returns the typeInstance, see SIGslotBase::typeInstance
   //
   virtual const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL >* typeInstance() const;

   //
   // nothing to do since there is no such trackable instance
   //
   virtual void onTrackerDestroy(SignalType* pOwner){};
   virtual void onTrackableDestroy(SignalType* pOwner, COLtrackable* pTrackable){};

private:
   TCallbackFunction pFunction;
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCallback)& operator=(const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCallback)&);
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCallback)(const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCallback)&);
};

template < SIG_SIGNALLER_TEMPLATE_PARAMS_IMPL >
const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL >* SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCallback)< SIG_SIGNALLER_PARAMS_IMPL >::typeInstance() const
{
   static SIG_SIGNALLER_TEMPLATE_NAME(SIGslotCallback)<SIG_SIGNALLER_PARAMS_IMPL > TypeInstance(0);
   return &TypeInstance;
}

//---------------------------------------------------------------------------
// SIGslotVoidMethod
//
// A slot that calls a member function along with an instance
// instances have to be COLtrackable.
//
//---------------------------------------------------------------------------
template < class _Class, SIG_SIGNALLER_TEMPLATE_PARAMS >
class SIG_SIGNALLER_TEMPLATE_NAME(SIGslotVoidMethod) :
     public SIG_SIGNALLER_TEMPLATE_NAME(SIGslotSingleBase)< SIG_SIGNALLER_PARAMS_IMPL >
{
private:
   typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL > SlotBaseType;
   typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotNull)< SIG_SIGNALLER_PARAMS_IMPL > SlotNullType;
   typedef SIGsignallerVoid SignalType;

public:
   typedef void (_Class::*TMethod)(SIG_SIGNALLER_PARAMS);
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotVoidMethod)(_Class* ipInstance,TMethod ipMethod)
      : pInstance(ipInstance),pMethod(ipMethod)
   {
   };
   virtual ~SIG_SIGNALLER_TEMPLATE_NAME(SIGslotVoidMethod)()
   {};

   //
   // Just call the member function.
   // We do not do a precondition check to avoid the runtime cost
   //
   virtual void operator()( SIG_SIGNALLER_PARAMS_NAMES_WITH_OWNER )
   {
      //note that we don't have to worry about callInProgress here, because if we do delete ourself,
      //we don't do anything after the function call....safe.
      (pInstance->*pMethod)( SIG_SIGNALLER_PARAMS_NAMES2 );
   }


   //
   // returns true if Rhs is the same type and points to the same function and instance
   //
   virtual bool operator==( const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL >& Orig)
   {
      if (Orig.typeInstance() == typeInstance())
      {  //safe to cast
         typedef SIG_SIGNALLER_TEMPLATE_NAME(SIGslotVoidMethod)< SIG_SIGNALLER_PARAMS_IMPL_WITH_CLASS > TThis;
         const TThis& Cast = (const TThis&)Orig;
         if (Cast.pInstance == this->pInstance && Cast.pMethod == this->pMethod)
         {
            return true;
         }
      }
      return false;
   }

   virtual const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL >* typeInstance() const;

   //
   // returns the attached instance, which must be COLtrackable
   //
   virtual COLtrackable* trackedInstance() { return this->pInstance; };

   //
   // If the trackable instance is going out of scope, and this slot is using it
   // it should destroy itself and switch the signal to null mode (implicit disconnect)
   //
   virtual void onTrackableDestroy(SignalType* pOwner,
                                   COLtrackable* pTrackable)
   {
      if (pInstance == pTrackable && pOwner)
      {
         //move back to null slot mode
         COLPRECONDITION(pOwner->pVoidSlotPrivate == this)
         pOwner->pVoidSlotPrivate = &SlotNullType::instance();
         delete this;
      }
   }

   //
   // If the SIGsignaller is going out of scope, it should notify the instance
   // its tracking to remove the signal from events
   //
   virtual void onTrackerDestroy(SIGsignallerVoid* pOwner)
   {
      COLPRECONDITION(this->pInstance != NULL)
      this->pInstance->removeTracker(pOwner);
   };

private:
   TMethod pMethod;
   _Class* pInstance;
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotVoidMethod)& operator=(const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotVoidMethod)&);
   SIG_SIGNALLER_TEMPLATE_NAME(SIGslotVoidMethod)(const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotVoidMethod)&);
};

template< class _Class, SIG_SIGNALLER_TEMPLATE_PARAMS_IMPL >
const SIG_SIGNALLER_TEMPLATE_NAME(SIGslotBase)< SIG_SIGNALLER_PARAMS_IMPL >* SIG_SIGNALLER_TEMPLATE_NAME(SIGslotVoidMethod)< _Class, SIG_SIGNALLER_PARAMS_IMPL >::typeInstance() const
{
   static SIG_SIGNALLER_TEMPLATE_NAME(SIGslotVoidMethod)<SIG_SIGNALLER_PARAMS_IMPL_WITH_CLASS> TypeInstance(0,0);
   return &TypeInstance;
}
