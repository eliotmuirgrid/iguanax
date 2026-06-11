//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODplaceComposite
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, September 14th, 2011 @ 09:37:28 AM
//
//---------------------------------------------------------------------------
#include "NODprecomp.h"
#pragma hdrstop

#include "NODplaceComposite.h"
#include "NODoperation.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void NODdenullify(NODplace& Child){
   if (!Child.isReadOnly() && !Child.isSimple() && Child.isNull()){
      Child.setIsNull(false);
   }
}

NODplaceComposite::NODplaceComposite(){
   COL_METHOD(NODplaceComposite::NODplaceComposite);
}

NODplaceComposite::~NODplaceComposite(){
   COL_METHOD(NODplaceComposite::~NODplaceComposite);
}

const COLref<NODplace> NODplaceComposite::createDummyItem() const{
   COLPRECONDITION(isVector());
   COLPRECONDITION(isHomogeneous());
   // The implementing class, on receiving -1 as the index, should not
   // modify itself.
   NODplace* pNewChild = const_cast<NODplaceComposite*>(this)->newChild(-1, NULL, NULL);
   pNewChild->setIsNull(true);
   return pNewChild;
}

int NODplaceComposite::size() const{
   return m_Children.size();
}

void NODplaceComposite::clear(){
   if (isReadOnly()){
      COL_ERROR_STREAM_PLAIN(COL_T("This tree is read-only"), NOD_IS_READ_ONLY);
   }
   if (isSimple()){
      setValue(NOD_EMPTY_SIMPLE_VALUE);
   } else if (isVector()){
      int CountOfItem = m_Children.size();
      for (int i = 0; i < CountOfItem; i++){
         onRemoveChild(i);
      }
      m_Children.resize(0);
   } else {// isComplex()
      int CountOfItem = m_Children.size();
      for (int i = 0; i < CountOfItem; i++){
         nullifyChildImpl(i);
      }
   }
}

void NODplaceComposite::remove(int i){
   if (isReadOnly()){
      COL_ERROR_STREAM_PLAIN(COL_T("This tree is read-only"), NOD_IS_READ_ONLY);
   } else if (isSimple()){
      COL_ERROR_STREAM_PLAIN(COL_T("This is a leaf node without children."), NOD_IS_LEAF);
   } else if (i < 0 || i >= m_Children.size()){
      COL_ERROR_STREAM_PLAIN(COL_T("Index ") << i << COL_T(" is out of bounds."), NOD_INDEX_OUT_OF_BOUNDS);
   }
   if (onRemoveChild(i)){
      m_Children.remove(i);
   } else {
      nullify(i);
   }
}

NODplace& NODplaceComposite::add(const char* Arg1, const char* Arg2){
   if (isReadOnly()){
      COL_ERROR_STREAM_PLAIN(COL_T("This tree is read-only"), NOD_IS_READ_ONLY);
   }else if (isSimple()){
      COL_ERROR_STREAM_PLAIN(COL_T("This is a leaf node to which children cannot be added."), NOD_IS_LEAF);
   }
   m_Children.push_back(newChild(m_Children.size(), Arg1, Arg2));
   NODplace& NewChild = *m_Children[m_Children.size()-1];
   NODdenullify(NewChild);
   return NewChild;
}

NODplace& NODplaceComposite::insert(int i, const char* Arg1, const char* Arg2){
   if (isReadOnly()){
      COL_ERROR_STREAM_PLAIN(COL_T("This tree is read-only"), NOD_IS_READ_ONLY);
   } else if (isSimple()){
      COL_ERROR_STREAM_PLAIN(COL_T("This is a leaf node to which children cannot be added."), NOD_IS_LEAF);
   }
   else if (i < 0 || i > m_Children.size()){
      COL_ERROR_STREAM_PLAIN(COL_T("Index ") << i << COL_T(" is out of bounds."), NOD_INDEX_OUT_OF_BOUNDS);
   }

   m_Children.insert(newChild(i, Arg1, Arg2), i);
   NODplace& NewChild = *m_Children[i];
   NODdenullify(NewChild);
   return NewChild;
}

NODplace& NODplaceComposite::child(int i){
   COL_METHOD(NODplaceComposite::child-nonconst);
   COL_VAR3(isReadOnly(), i, m_Children.size());
   if (isSimple()){
      COL_ERROR_STREAM_PLAIN(COL_T("This is a leaf node without children."), NOD_IS_LEAF);
   } else if (i < 0){
      COL_ERROR_STREAM_PLAIN(COL_T("Index ") << i << COL_T(" is out of bounds."), NOD_INDEX_OUT_OF_BOUNDS);
   } else if (i >= m_Children.size()){
      if (isComplex() || !isHomogeneous() || isReadOnly()){
         COL_ERROR_STREAM_PLAIN(COL_T("Index ") << i << COL_T(" is out of bounds."), NOD_INDEX_OUT_OF_BOUNDS);
      } else {// Writable Homogeneous Vector
         resizeHomogeneousVector(i+1);
      }
   }

   COLref<NODplace>& pChild = m_Children[i];
   if (pChild.get()){
      NODdenullify(*pChild);
      return *pChild;
   } else {
      m_Children[i] = initChild(i);
      COLASSERT(m_Children[i].get());
      NODplace& Child = *m_Children[i];
      NODdenullify(Child);
      return Child;
   }
}

const NODplace& NODplaceComposite::child(int i) const{
   COL_METHOD(NODplaceComposite::child-const);
   COL_VAR2(i, m_Children.size());
   if (isSimple()){
      COL_ERROR_STREAM_PLAIN(COL_T("This is a leaf node without children."), NOD_IS_LEAF);
   } else if (i < 0){
      COL_ERROR_STREAM_PLAIN(COL_T("Index ") << i << COL_T(" is out of bounds."), NOD_INDEX_OUT_OF_BOUNDS);
   } else if (i >= m_Children.size()){
      COL_ERROR_STREAM_PLAIN(COL_T("Index ") << i << COL_T(" is out of bounds."), NOD_INDEX_OUT_OF_BOUNDS);
   }
   COLref<NODplace>& pChild = m_Children[i];
   if (pChild.get()){
      return *pChild;
   } else{
      m_Children[i] = initChild(i);
      COLPRECONDITION(m_Children[i].get());
      return *m_Children[i];
   }
}

NODplace& NODplaceComposite::child(const COLstring& Name){
   if (isSimple()){
      COL_ERROR_STREAM_PLAIN(COL_T("This is a leaf node without children."), NOD_IS_LEAF);
   } else if (isVector()){
      COL_ERROR_STREAM_PLAIN(COL_T("This node does not have named members."), NOD_NOT_COMPLEX_TYPE);
   }
   int i = childIndex(Name);
   if (i < 0){
      NODthrowNoSuchMember(*this, Name);
   }
   COLref<NODplace>& pChild = m_Children[i];
   if (pChild.get()){
      NODdenullify(*pChild);
      return *pChild;
   }else{
      m_Children[i] = initChild(i);
      COLPRECONDITION(m_Children[i].get());
      NODplace& Child = *m_Children[i];
      NODdenullify(Child);
      return Child;
   }
}

const NODplace& NODplaceComposite::child(const COLstring& Name) const
{
   if (isSimple()){
      COL_ERROR_STREAM_PLAIN(COL_T("This is a leaf node without children."), NOD_IS_LEAF);
   } else if (isVector()){
      COL_ERROR_STREAM_PLAIN(COL_T("This node does not have named members."), NOD_NOT_COMPLEX_TYPE);
   }

   int i = childIndex(Name);
   if (i < 0){
      NODthrowNoSuchMember(*this, Name);
   }

   COLref<NODplace>& pChild = m_Children[i];
   if (pChild.get()){
      return *pChild;
   } else {
      m_Children[i] = initChild(i);
      COLPRECONDITION(m_Children[i].get());
      return *m_Children[i];
   }
}

// Default implementation which is probably nearly as good as any overridden implementation.
int NODplaceComposite::childIndex(const COLstring& Name) const{
   if (isComplex()){
      int CountOfChild = m_Children.size();
      for (int i = 0; i < CountOfChild; i++){
         const NODplace& Child = child(i);
         if (Child.isNamed() && Child.name() == Name){
            return i;
         }
      }
   }
   return -1;
}

void NODplaceComposite::remove(const COLstring& Name){
   if (isReadOnly()){
      COL_ERROR_STREAM_PLAIN(COL_T("This tree is read-only"), NOD_IS_READ_ONLY);
   } else if (isSimple()){
      COL_ERROR_STREAM_PLAIN(COL_T("This is a leaf node without children."), NOD_IS_LEAF);
   } else if (isVector()){
      COL_ERROR_STREAM_PLAIN(COL_T("This node does not have named members."), NOD_NOT_COMPLEX_TYPE);
   }
   int i = childIndex(Name);
   if (i < 0){
      NODthrowNoSuchMember(*this, Name);
   }
   if (onRemoveChild(i)){
      m_Children.remove(i);
   } else {
      nullify(i);
   }
}

void NODplaceComposite::nullify(int i){
   if (isReadOnly()){
      COL_ERROR_STREAM_PLAIN(COL_T("This tree is read-only"), NOD_IS_READ_ONLY);
   } else if (isSimple()){
      COL_ERROR_STREAM_PLAIN(COL_T("This is a leaf node without children."), NOD_IS_LEAF);
   } else if (i < 0 || i >= m_Children.size()){
      COL_ERROR_STREAM_PLAIN(COL_T("Index ") << i << COL_T(" is out of bounds."), NOD_INDEX_OUT_OF_BOUNDS);
   }
   nullifyChildImpl(i);
}

void NODplaceComposite::nullify(const COLstring& Name){
   if (isReadOnly()){
      COL_ERROR_STREAM_PLAIN(COL_T("This tree is read-only"), NOD_IS_READ_ONLY);
   }
   else if (isSimple()){
      COL_ERROR_STREAM_PLAIN(COL_T("This is a leaf node without children."), NOD_IS_LEAF);
   }
   else if (isVector()){
      COL_ERROR_STREAM_PLAIN(COL_T("This node does not have named members."), NOD_NOT_COMPLEX_TYPE);
   }

   int i = childIndex(Name);
   if (i < 0){
      NODthrowNoSuchMember(*this, Name);
   }

   nullifyChildImpl(i);
}

void NODplaceComposite::setSize(int Size){
   m_Children.clear();
   m_Children.resize(Size);
}

void NODplaceComposite::insertChildCache(int i){
   COLPRECONDITION(i >= 0 && i <= m_Children.size());
   m_Children.insert(NULL, i);
}

void NODplaceComposite::insertChildCache(int i, NODplace* pChild){
   COLPRECONDITION(i >= 0 && i <= m_Children.size());
   m_Children.insert(pChild, i);
}

void NODplaceComposite::removeChildCache(int i){
   COLPRECONDITION(i >= 0 && i < m_Children.size());
   m_Children.remove(i);
}

void NODplaceComposite::nullifyChildImpl(int i){
   onNullifyChild(i);
   COLref<NODplace>& pChild = m_Children[i];
   if (pChild.get() && !pChild->isNull()){
      pChild->clear();
      pChild->setIsNull(true);
   }
}

void NODplaceComposite::resizeHomogeneousVector(int NewSize){
   int OldSize = m_Children.size();
   m_Children.resize(NewSize);
   for (int i = OldSize; i < NewSize; i++){
      m_Children[i] = newChild(i, NULL, NULL);
   }
}
