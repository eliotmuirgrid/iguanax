//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLhashmap
//
// Description:
//
// Templated hash map collection class with unique keys
//
// Author: Ian Farquharson
// Date:   02/05/2004
//---------------------------------------------------------------------------
#include "COLerror.h"
#include "COLhashmap.h"

const COLindex COL_MAX_CHAIN_LENGTH = 4; // when number of nodes in a bucket exceeds this, resize Buckets

void COLvoidLookup::init(void)
{
   // A series of prime numbers; must end with 0.
   static const COLindex BucketArraySizes[] =
   {
      2, 5, 11, 17, 29, 47, 71, 107, 163, 251, 379, 569, 857, 1289,
      1949, 2927, 4391, 6599, 9901, 14867, 22303, 33457, 50207,
      75323, 112997, 169501, 254257, 381389, 572087, 858149, 0
   };

   Size = 0;
   Buckets = new COLhashmapPlace[2];
   Buckets[0] = NULL;
   Buckets[1] = NULL;
   pCountOfBucket = &BucketArraySizes[0]; // == 2
}

COLvoidLookup::~COLvoidLookup(void)
{
   delete[] Buckets;
}

void COLvoidLookup::clear(void)
{
   COLhashmapPlace CurrentPlace;
   const COLindex CountOfBucket = *pCountOfBucket;
   for (COLindex BucketIndex = 0; BucketIndex < CountOfBucket; BucketIndex++)
   {
      CurrentPlace = Buckets[BucketIndex];
      while (CurrentPlace != NULL)
      {
         COLhashmapPlace Place = CurrentPlace;
         CurrentPlace = CurrentPlace->Next;
         destroyItem(Place);
      }
      Buckets[BucketIndex] = NULL;
   }
   try
   {
      COLhashmapPlace* OldBuckets = Buckets;
      init();
      delete[] OldBuckets;
   }
   catch (...)
   {
      Size = 0;
      // suppress exception because it is not fatal - no memory
   }
   invariant();
}

void COLvoidLookup::remove(COLhashmapPlace Place)
{
   COLPRECONDITION(Place != NULL);
   if (Place == NULL)
   {
      return; // paranoid check
   }
   COLindex BucketIndex = hashItem(Place) % *pCountOfBucket;
   if (Place == Buckets[BucketIndex])
   {
      Buckets[BucketIndex] = Place->Next;
   }
   else
   {
      for (COLhashmapPlace Current = Buckets[BucketIndex]; Current != NULL; Current = Current->Next)
      {
         if (Current->Next == Place)
         {
            Current->Next = Place->Next;
            break;
         }
         COLPRECONDITION(Current->Next != NULL);
      }
   }
   destroyItem(Place);
   Size--;
   invariant();
}

COLhashmapPlace COLvoidLookup::first(void) const
{
   COLhashmapPlace Current;
   const COLindex CountOfBucket = *pCountOfBucket;
   for (COLindex BucketIndex = 0; BucketIndex < CountOfBucket; BucketIndex++)
   {
      if ((Current = Buckets[BucketIndex]) != NULL)
      {
         return Current;
      }
   }
   return NULL;
}

COLhashmapPlace COLvoidLookup::last(void) const
{
   for (COLindex BucketIndex = *pCountOfBucket; BucketIndex > 0; BucketIndex--)
   {
      for (COLhashmapPlace Current = Buckets[BucketIndex - 1]; Current != NULL; Current = Current->Next)
      {
         if (Current->Next == NULL)
         {
            return Current;
         }
      }
   }
   return NULL;
}

COLhashmapPlace COLvoidLookup::previous(COLhashmapPlace Place) const
{
   COLindex BucketIndex = (Place == NULL ? *pCountOfBucket : hashItem(Place) % *pCountOfBucket);
   if (Place != NULL && Place != Buckets[BucketIndex])
   {
      for (COLhashmapPlace Current = Buckets[BucketIndex]; Current != NULL; Current = Current->Next)
      {
         if (Current->Next == Place)
         {
            Place = Current;
            break;
         }
         COLPRECONDITION(Current->Next != NULL);
      }
   }
   else
   {
      Place = NULL;
      for (; BucketIndex > 0; BucketIndex--)
      {
         if (Buckets[BucketIndex - 1] != NULL)
         {
            for (COLhashmapPlace Current = Buckets[BucketIndex - 1]; Current != NULL; Current = Current->Next)
            {
               if (Current->Next == NULL)
               {
                  Place = Current;
                  break;
               }
            }
            break;
         }
      }
   }
   return Place;
}

COLhashmapPlace COLvoidLookup::next(COLhashmapPlace Place) const
{
   if (Place != NULL && Place->Next != NULL)
   {
      Place = Place->Next;
   }
   else
   {
      const COLindex CountOfBucket = *pCountOfBucket;
      COLindex BucketIndex = (Place == NULL ? (COLindex)-1 : COLindex(hashItem(Place) % CountOfBucket));
      Place = NULL;
      for (BucketIndex++; BucketIndex < CountOfBucket; BucketIndex++)
      {
         if ((Place = Buckets[BucketIndex]) != NULL)
         {
            break;
         }
      }
   }
   return Place;
}

bool COLvoidLookup::removeItem(COLindex Hash, const void* Key)
{
   COLhashmapPlace Current = findItem(Hash, Key);
   if (Current != NULL)
   {
      remove(Current);
      return true;
   }
   return false;
}

inline void COLvoidLookup::grow(void)
{
   const COLindex NewCountOfBucket = *(pCountOfBucket+1);
   COLhashmapPlace* NewBuckets = 0;
   
   if( NewCountOfBucket )
   {
      try
      {
         NewBuckets = new COLhashmapPlace[NewCountOfBucket];
      }
      catch (...)
      {
         // Not enough memory, so we won't increase the number of buckets.
      }
   }

   if( NewBuckets )
   {
      COLindex CountOfBucket = *pCountOfBucket;

      COLhashmapPlace Chain = NULL;
      COLhashmapPlace Current, Place;
      COLindex       Bucket;

      // Insert all items into a temporary Chain

      for (Bucket = 0; Bucket < CountOfBucket; Bucket++)
      {
         Current = Buckets[Bucket];
         while (Current != NULL)
         {
            Place = Current;
            Current = Current->Next;
            Place->Next = Chain;
            Chain = Place;
         }
      }

      // Switch over to the new, larger bucket array.

      delete[] Buckets;
      Buckets = NewBuckets;
      ++pCountOfBucket;

      CountOfBucket = NewCountOfBucket;

      // Set all bucket pointers to NULL

      for (Bucket = 0; Bucket < CountOfBucket; Bucket++)
      {
         Buckets[Bucket] = NULL;
      }

      // Reinsert all the existing items

      while (Chain != NULL)
      {
         Place = Chain;
         Chain = Chain->Next;
         Bucket = hashItem(Place) % CountOfBucket;
         Place->Next = Buckets[Bucket];
         Buckets[Bucket] = Place;
      }
   }
}

COLhashmapPlace COLvoidLookup::addItem(COLindex Hash, const void* Key, COLhashmapPlace Node)
{
   if (Node == NULL)
   {
      COL_ERROR_STREAM("Out of memory", COLerror::SystemError);
   }

   COLindex CountOfBucket = *pCountOfBucket;

   COLindex BucketIndex = Hash % CountOfBucket;
   COLindex ChainLength = 0;

   // DiagCheck for an existing Item that matches the Key

   for (COLhashmapPlace Place = Buckets[BucketIndex]; Place != NULL; Place = Place->Next)
   {
      if (equalKey(Key, Place))
      {
         destroyItem(Node); // clean up the duplicate Item
         return NULL;
      }
      ChainLength++;
   }

   // If the chain length is getting long then upsize the bucket array

   if (ChainLength >= COL_MAX_CHAIN_LENGTH && Size >= CountOfBucket)
   {
      grow();
      CountOfBucket = *pCountOfBucket;
      BucketIndex = Hash % CountOfBucket;
   }

   // Insert the new Item

   Node->Next = Buckets[BucketIndex];
   Buckets[BucketIndex] = Node;
   Size++;
   invariant();
   return Node;
}

COLhashmapPlace COLvoidLookup::findItem(COLindex Hash, const void* Key) const
{
   COLindex BucketIndex = Hash % *pCountOfBucket;
   for (COLhashmapPlace Current = Buckets[BucketIndex]; Current != NULL; Current = Current->Next)
   {
      if (equalKey(Key, Current))
      {
         return Current;
      }
   }
   return NULL;
}

COLindex COLvoidLookup::countOfBucket() const
{
   return *pCountOfBucket;
}

COLindex COLvoidLookup::countOfBucketItem(COLindex BucketIndex) const
{
   COLindex NodeCount = 0;
   for (COLhashmapPlace Current = Buckets[BucketIndex]; Current != NULL; Current = Current->Next)
   {
      NodeCount++;
   }
   return NodeCount;
}

#ifdef COL_ENABLE_INVARIANT_INTEGRITY_CHECK
void COLvoidLookup::invariant(void) const
{
   const COLindex CountOfBucket = *pCountOfBucket;

   COLindex NodeCount = 0;
   for (COLindex BucketIndex = 0; BucketIndex < CountOfBucket; BucketIndex++)
   {
      for (COLhashmapPlace Current = Buckets[BucketIndex]; Current != NULL; Current = Current->Next)
      {
         COLASSERT(hashItem(Current) % CountOfBucket == (COLindex) BucketIndex); // Item is in correct bucket
         NodeCount++;
      }
   }
   COLASSERT(Size == NodeCount);
}
#endif

