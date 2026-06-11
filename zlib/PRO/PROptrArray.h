#ifndef __PRO_PTR_ARRAY_H__
#define __PRO_PTR_ARRAY_H__
//------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  PROptrArray
// 
// Description
//
// Templated class which simplifies use of a dynamic, null terminated pointer array
// This is useful for environment blocks and command argument arrays.
//
// Author: Eliot Muir
// Date:   Fri 05/17/2002 
//---------------------------------------------------------------------------
#include <COL/COLerror.h>

template <class _T>
class PROptrArray {
public:
   PROptrArray(): Size(0), Capacity(0) {
      Array = new _T*[1];
      Array[0] = 0; //null terminate
   }

   virtual ~PROptrArray() {
      delete[] Array;
   }

   //set and get values with the [] operator
   _T*& operator[](int ItemIndex) {
      COLPRECONDITION(ItemIndex < Size);
      return Array[ItemIndex];   
   }

   void add(const _T* Value) {
      resize(Size + 1);
      Array[Size - 1] = (_T*) Value;
      Array[Size] = 0;
   }

   //does not include null terminating element
   int size() {
      return Size;
   }

   void clear() {
      //null all entries
      for (int ItemIndex=0; ItemIndex <= Size; ItemIndex++) {
         Array[ItemIndex] = 0;
      }
   }

   void resize(int NewSize) {
      if (NewSize < Size || NewSize <= Capacity) {
         Size = NewSize;
         Array[Size] = 0;
         return;
      } else {
         //decide new size
         int NewCapacity = Capacity;
         if (!NewCapacity) {
            NewCapacity++;
         }
         //grow geometrically
         while (NewCapacity < NewSize) {
            NewCapacity = NewCapacity * 2;
         }
         //make new array
         _T** pNewArray = new _T*[NewCapacity+1]; //room for null character
         Capacity = NewCapacity;
         Size = NewSize;
         int ItemIndex;
         //null all entries
         for (ItemIndex=0; ItemIndex <= Capacity; ItemIndex++) {
            pNewArray[ItemIndex] = 0;
         }
         //copy pointers
         ItemIndex = 0;
         while (Array[ItemIndex]) {
            pNewArray[ItemIndex] = Array[ItemIndex];
            ItemIndex++;
         }
         delete[] Array;
         Array = pNewArray;
      }
   }

   _T** const array() const {
      return Array;
   }

private:
   _T** Array;
   int Size;
   int Capacity;
};

#endif // end of defensive include
