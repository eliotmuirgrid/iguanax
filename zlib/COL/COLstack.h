#ifndef __COL_STACK_H__
#define __COL_STACK_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: COLstack
//
// Description:
//
// LIFO class similar to STL stack.  Pass on your first read of COL.
//
// The objects stored in the template must support the copy constructor
// Type(const Type&)
//---------------------------------------------------------------------------

#include "COLlist.h"

template <class Type>
class COLstack : private COLlist<Type>{
public:
   COLstack()              {}
   virtual ~COLstack()     {}

   void clear()            { COLlist<Type>::clear(); }

   // STL-like methods

   // Number of elements in the stack.
   COLindex size() const   { return COLlist<Type>::size(); }

   // Is the stack empty?
   bool empty() const      { return size() == 0; }

   // Get the "newest" element in the stack, i.e., the most recently push()ed element.
   // Undefined if stack is empty().
   Type& top()             { return COLlist<Type>::operator[](this->last()); }
   const Type& top() const { return COLlist<Type>::operator[](this->last()); }

   // Push an item on to the stack.
   Type& push(const Type& Item) { return COLlist<Type>::operator[](this->add(Item)); } 
   Type& push()                 { return COLlist<Type>::operator[](this->add()); }

   // Pop an item from the stack.
   // Undefined if stack is empty().
   void pop()              { COLlist<Type>::remove(this->last()); }
};

#endif // end of defensive include

