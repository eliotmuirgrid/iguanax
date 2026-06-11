#ifndef __COL_REF_H__
#define __COL_REF_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLref
//
// Description: Intrusive ref counted smart pointer class
//
// Object pointers managed by COLref must be derived from COLrefCounted.
// COLref calls AddRef() and Release() on the object to manage the ref count.
// COLref has assignment, construction and destruction operations.
//
// Note: For usage sample and more information, see intranet:
// Developer Information -> C++ Library Documentation -> COL - Core Library,
// in the "Reference Counted Classes" section.
//
// Author: Eliot Muir
//---------------------------------------------------------------------------
#include "COLrefCounted.h"
#include "COLerror.h"

template<class R>
class COLref{
public:
   COLref(R* Reference = NULL);
   ~COLref();

   R* operator=(R* Reference);

   R& operator*();
   const R& operator*() const;
   R* operator->();
   const R* operator->() const;

   R* get() const { return m_Ptr; }

   operator const R*() const { return m_Ptr; }

   int refcount() const { return m_Ptr ? m_Ptr->refcount() : 0; }

   COLref<R>& operator=(const COLref<R>& Orig);  
   COLref(const COLref<R>& Orig);
private:
   R* m_Ptr;
};


template<class R>
inline COLref<R>::COLref(const COLref<R>& Orig) {
   m_Ptr = Orig.m_Ptr;
   if (m_Ptr != NULL)
   {
      m_Ptr->AddRef();
   }
}

template<class R>
inline COLref<R>::COLref(R* Reference) {
   m_Ptr = Reference;
   if (m_Ptr != NULL)
   {
      m_Ptr->AddRef();
   }
}

template<class R>
inline COLref<R>::~COLref() {
   try {
      if (m_Ptr) {
         m_Ptr->Release();
         m_Ptr=0;
      }
   }
   catch (const COLerror& Error) {
      //COL_LOG(Error);
   }
}

template<class R>
inline R* COLref<R>::operator=(R* pReference) {
   // This has to go before - imagine if pReference == m_Ptr
   if (pReference != NULL) {
      pReference->AddRef();
   }
   if (m_Ptr) {
      m_Ptr->Release();
   }
   m_Ptr = pReference;
   return m_Ptr;
}

template<class R>
inline  R& COLref<R>::operator*() {
   return *m_Ptr;
}

template<class R>
inline const R& COLref<R>::operator*() const {
   return *m_Ptr;
}


template<class R>
inline R* COLref<R>::operator->() {
   return m_Ptr;
}

template<class R>
inline const R* COLref<R>::operator->() const {
   return m_Ptr;
}

template<class R>
inline COLref<R>& COLref<R>::operator=(const COLref<R>& Orig) {
   if (Orig.m_Ptr) {
      Orig.m_Ptr->AddRef();
   }
   if (m_Ptr) {
      m_Ptr->Release();
   }
   m_Ptr = Orig.m_Ptr;
   return *this;
}

#endif // end of defensive include
