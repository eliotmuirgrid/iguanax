#ifndef __XML_PTR_ARRAY_H__
#define __XML_PTR_ARRAY_H__
//------------------------------------------------------------------------
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  XMLptrArray
// 
// Description
//
// Templated class which simplifies use of a dynamic, null terminated pointer array
// This is used to rebuild arrays for transcoding.
//
// Note that a similiar class exists in PIP
//
// Author: Nasron Cheong
// Date:   May 2005 
//
//---------------------------------------------------------------------------
#include <COL/COLminimumInclude.h>

template < class Type >
class XMLptrArray
{
public:
   XMLptrArray(): Size(0), Capacity(0)
   {
      Array = new Type*[1];
      Array[0] = 0; //null terminate
   }

   virtual ~XMLptrArray()
   {
      delete[] Array;
   }

   //set and get values with the [] operator
   Type*& operator[](COLuint32 Index)
   {
      COLPRECONDITION(Index < Size);
      return Array[Index];   
   }

   void add(const Type* Value)
   {
      resize(Size + 1);
      Array[Size - 1] = (Type*) Value;
      Array[Size] = 0;
   }

   //does not include null terminating element
   COLuint32 size()
   {
      return Size;
   }

   void clear()
   {
      //null all entries
      for (COLuint32 Index = 0; Index <= Size; Index++)
      {
         Array[Index] = 0;
      }
   }

   void resize(COLuint32 NewSize)
   {
      if (NewSize < Size || 
          NewSize <= Capacity)
      {
         Size = NewSize;
         Array[Size] = 0;
         return;
      }
      else
      {
         //decide new size
         COLuint32 NewCapacity = Capacity;
         if (!NewCapacity)
         {
            NewCapacity++;
         }
         //grow geometrically
         while (NewCapacity < NewSize)
         {
            NewCapacity = NewCapacity * 2;
         }
         //make new array
         Type** pNewArray = new Type*[NewCapacity+1]; //room for null character
         Capacity = NewCapacity;
         Size = NewSize;
         COLuint32 Index = 0;
         //null all entries
         for (; Index <= Capacity; Index++)
         {
            pNewArray[Index] = 0;
         }
         //copy pointers
         Index = 0;
         while (Array[Index])
         {
            pNewArray[Index] = Array[Index];
            Index++;
         }
         delete[] Array;
         Array = pNewArray;
      }
   }

   Type** const array() const
   {
      return Array;
   }

private:
   Type** Array;
   COLuint32 Size;
   COLuint32 Capacity;
};

#endif // end of defensive include
