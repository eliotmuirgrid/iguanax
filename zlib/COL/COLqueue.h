#ifndef __COL_QUEUE_H__
#define __COL_QUEUE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: COLqueue
//
// Description:
//
// FIFO
//
// This is not too clever implementation of a queue.  It's a thin
// layer around COLlist which is a double linked list class.   I have
// wrapped COLlist in order to make it easier in the future to come
// up with a much smarter queue class.
//
// The objects stored in the template must support the copy constructor
// Type(const Type&)
//
// Author: Eliot Muir
// Date:   Tue 07/27/2004 
//---------------------------------------------------------------------------

#include "COLlist.h"

template <class Type>
class COLqueue : private COLlist<Type>{
public:
   COLqueue () {}
   virtual ~COLqueue () {}

   Type& push_back(const Type& NewItem) { return COLlist<Type>::operator[](this->add(NewItem)); } 
   Type& push_back() { return COLlist<Type>::operator[](this->add()); }

   void pop_front() { COLlist<Type>::remove(this->first()); }

   void clear() { COLlist<Type>::clear(); }

   // STL-like methods

   // Number of elements in the queue.
   COLindex size() const { return COLlist<Type>::size(); }

   // Is the queue empty?
   bool empty() const    { return size() == 0; }

   // Get the "newest" element in the queue, i.e., the most recently push()ed element.
   // Undefined if queue is empty().
   Type& back()             { return COLlist<Type>::operator[](this->last()); }
   const Type& back() const { return COLlist<Type>::operator[](this->last()); }

   // Get the "oldest" element in the queue.
   // The same element that is popped out from the queue when pop() is called.
   // Undefined if queue is empty().
   Type& front()             { return COLlist<Type>::operator[](this->first()); }
   const Type& front() const { return COLlist<Type>::operator[](this->first()); }

   // Push item on to the back of the queue.
   Type& push(const Type& NewItem) { return COLlist<Type>::operator[](this->add(NewItem)); } 
   Type& push()                    { return COLlist<Type>::operator[](this->add()); }

   // Pop an item from the front of the queue.
   // Undefined if queue is empty().
   void pop()            { COLlist<Type>::remove(this->first()); }
};

#endif // end of defensive include

