#ifndef __LEG_ITERATOR_H__
#define __LEG_ITERATOR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LEGiterator
//
// Description:
//
// DEPRECATED - DO NOT USE.  The new collection classes do not need iterators
// since place holders for the same function in a more light weight manner.
//
// Abstract base iterator class for collections in the COL library.
//
// Known subclasses: LEGrefHashTableIterator
//
// Author: Rob Moyse
// Date:   Nov 3, 2003
//
//---------------------------------------------------------------------------

// TODO - this class is probably slated to become legacy since we
// made the design decision towards place holders rather than iterators
// in our new container classes.

#include <COL/COLminimumInclude.h>

template<class _Key, class _Value>
class LEGiterator
{

public:

   /**
    *   Move to the next item in the collection.  Returns
    * false if no more items in the collection, else
    * returns true.
    */
   virtual bool iterateNext() = 0;

   /**
    *   Move to the next item in the collection and place
    * the key and value in the passed-in parameters. Returns
    * false if no more items in the collection, else
    * returns true.
    */
   virtual bool iterateNext(_Key& Key, _Value& Value) = 0;

   /**
    * Reset the iterator to the first item in the
    * collection.
    */
   virtual void resetIterator() = 0;

   /**
    * Get the key for the item currently referenced
    * by this iterator.
    */
   virtual const _Key& key() = 0;

   /**
    * Get the value for the item currently referenced
    * by this iterator.
    */
   virtual _Value& value() = 0;

};

#endif // end of defensive include
