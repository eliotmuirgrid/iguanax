#ifndef __COL_AUTO_H__
#define __COL_AUTO_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLauto
//
// Description:
//
// Smart Owner Pointer Class.
//
// {
//    COLauto<Foo> pFoo = new Foo();
//
//    // when pFoo goes out of scope it will delete the Foo() object
//    // since it owns it.
// } <--- Foo deleted here.
//
// Note that it can be dangerous to use this in a collection class.
// For example, COLarray< COLauto<X> >.
// It will technically work, unless you do the following:
//
// COLarray< COLauto<X> > V;
// V.push_back(new X);
// COLauto<X> pX = V[0]; // bad bad bad!  Now when pX goes out of scope,
//                       // V[0] will have a bad pointer.
// const X* pX = V[0].get(); // this is better, if you absolutely must
//                           // use COLarray< COLauto<X> >.
//
// See http://stackoverflow.com/questions/111478/why-is-it-wrong-to-use-stdauto-ptr-with-standard-containers.
//
// Eliot - I recommend reading this carefully and understanding how ownership of
// that which is pointed to gets tranfered from one COLauto to another with the assignment
// operator.
//
// Author: Ryan Dewsbury
// Date:   4 August 2000
//---------------------------------------------------------------------------

#include "COLerror.h"

template<class TemplateClass>
class COLauto {
public:
   COLauto() : pObject(NULL), IsOwner(false) {}
   
   COLauto(TemplateClass* ipObject, bool iIsOwner = true)
    : pObject(ipObject), IsOwner(iIsOwner) {}
   
   // use this to assign what COLauto points to
   // NOTE: If you'd like to make COLauto point
   // to something it doesn't own, call setIsOwner(false);
   // after calling this function.
   COLauto<TemplateClass>& operator=(TemplateClass* ipObject) { 
      cleanUpPointer();
      pObject = ipObject; 
      IsOwner = true; 
      return *this; 
   }
   
   COLauto(const COLauto<TemplateClass>& Pointer)
   : pObject(Pointer.pObject),IsOwner(Pointer.IsOwner){ 
      ((COLauto<TemplateClass>&) Pointer).IsOwner = false; 
   }
   
   COLauto<TemplateClass>& operator=(const COLauto<TemplateClass>& Pointer) {
      if (&Pointer != this) {
         cleanUpPointer();
         pObject = Pointer.pObject;
         IsOwner = Pointer.IsOwner;
         ((COLauto< TemplateClass >&)Pointer).IsOwner = false;
      }
      return *this;
   }

   ~COLauto() { 
      try {
         cleanUpPointer();  
      } catch (const COLerror& Error) {
         //COL_LOG(Error);
      }
   }

   //access operators
   TemplateClass& operator *()             { return *pObject; }
   const TemplateClass& operator *() const { return *pObject; } 
   operator TemplateClass&()               { return *pObject; } 
   operator const TemplateClass&() const   { return *pObject; }  
   
   operator TemplateClass*()               { return pObject; } 
   operator const TemplateClass*() const   { return pObject; }  
   
   TemplateClass* operator->() { 
      COLASSERT(pObject); // #10840 it seems a crash is more useful
      return pObject; 
   }
   
   const TemplateClass* operator->() const { 
      COLASSERT(pObject); // #10840 it seems a crash is more useful
      return pObject; 
   }

   const TemplateClass* get() const { return pObject; }
   TemplateClass*  get()            { return pObject; }
   bool isOwner() const             { return IsOwner; }
   void setIsOwner(bool BeOwner)    { IsOwner = BeOwner; }

   TemplateClass* release() {
      if (IsOwner) {
         TemplateClass* p = pObject;
         IsOwner = false;
         pObject = 0;
         return p;
      }
      return 0;
   }

private:
   void cleanUpPointer()
   { 
      if( IsOwner ) {
         delete pObject; 
         pObject=0;
      }
   }

   bool IsOwner;
   TemplateClass* pObject;
};

#endif // end of defensive include
