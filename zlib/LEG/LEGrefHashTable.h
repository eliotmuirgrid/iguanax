#ifndef __LEG_REF_HASH_TABLE_H__
#define __LEG_REF_HASH_TABLE_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LEGrefHashTable
//
// Description:
//
// THIS IS DEPRECATED - DO NOT USE.
//
// Simple templated reference hash table class
//
// The key must have a hash function defined of the form
// LEGhashFunc(const Key& KeyValue);
//
// Author: Eliot Muir
// Date:   Tue 08/25/1998 
//
//
//---------------------------------------------------------------------------

#include <COL/COLerror.h>
#include <LEG/LEGiterator.h>
#include <LEG/LEGrefVect.h>
#include <COL/COLminimumInclude.h>

//Hash Functions

//Generic
// TODO change all size_t to COlindex - don't bother it's all legacy!
template <class PKey>
size_t LEGhashFunc(const PKey& Value)
{
   return (size_t)Value;
}

//COLstring specialization
template<>
size_t LEGhashFunc< COLstring >(const COLstring& String);

//COLuint64 Specialization
template<>
size_t LEGhashFunc< COLuint64>(const COLuint64& Value);
template <>
size_t LEGhashFunc< COLint64 >(const COLint64& Value);

template<class PKey, class PValue>
class LEGpair 
{
public:
   LEGpair() {}
   LEGpair(const PKey& nKey, const PValue& nValue)
           : Key(nKey), Value(nValue) {}
   ~LEGpair() {}
   PKey   Key;
   PValue Value;
};

template<class PKey, class PValue> class LEGrefHashTableIterator;

template<class PKey, class PValue>
class LEGrefHashTable 
{
public:
   LEGrefHashTable(size_t CountOfBucket=10);
   virtual ~LEGrefHashTable();

   //void showBuckets() const;

   PValue& operator[](const PKey& Key);
   const PValue& operator[](const PKey& Key) const;

   LEGpair<PKey,PValue>* findPair(const PKey& Key) const;
   bool has(const PKey& Key) const;

   PValue* getValue(const PKey& Key) const;
   const PKey* getKey(const size_t KeyIndex) const;

   void clear();

   void remove(const PKey& Key);

   void insert(const PKey& Key, const PValue& Value);

   size_t size() const { return m_Size; }

   friend class LEGrefHashTableIterator<PKey, PValue>;
   typedef LEGrefHashTableIterator<PKey, PValue> Iterator;
private:
   void removeAll();
   void init(size_t CountOfBucket);

   typedef LEGrefVect< LEGpair<PKey, PValue>* > COLbucket;

   LEGrefVect<COLbucket*> m_Bucket;
   LEGrefVect< PKey* > m_Keys;

   void findIndex(const PKey& Key, size_t& BucketIndex, size_t& ItemIndex) const;
   size_t m_Size;

   LEGrefHashTable(const LEGrefHashTable<PKey,PValue>& Original) {}
   LEGrefHashTable<PKey,PValue>& operator=(const LEGrefHashTable<PKey,PValue>& Original) { return *this;}
};

template<class PKey, class PValue>
class LEGrefHashTableIterator : public LEGiterator<PKey, PValue>
{
public:
   LEGrefHashTableIterator(const LEGrefHashTable<PKey, PValue>& Table);
   ~LEGrefHashTableIterator() {}

   void resetIterator();
   bool iterateNext(PKey& Key, PValue& Value);

   //a more efficient iteration style, no copy of key or value is made
   bool iterateNext();
   const PKey& key();
   PValue& value();
private:
   size_t m_IterBucketIndex;
   size_t m_IterItemIndex;
   const LEGrefHashTable<PKey, PValue>* m_pTable;
};

template<class PKey, class PValue>
LEGrefHashTableIterator<PKey, PValue>::LEGrefHashTableIterator
(
   const LEGrefHashTable<PKey, PValue>& Table
)
{
   m_pTable = &Table;
   resetIterator();
}

template<class PKey, class PValue>
void LEGrefHashTableIterator<PKey, PValue>::resetIterator()
{
   m_IterBucketIndex=npos;
   m_IterItemIndex=0;
}

template<class PKey, class PValue>
bool LEGrefHashTableIterator<PKey,PValue>::iterateNext
(
    PKey& Key, 
    PValue& Value
)
{
   if (m_IterBucketIndex == npos)
   {
      m_IterBucketIndex = 0;
   }
   while (m_IterBucketIndex < m_pTable->m_Bucket.size() && 
           m_pTable->m_Bucket[m_IterBucketIndex]->size() <= m_IterItemIndex )
   {
      m_IterBucketIndex++;
      m_IterItemIndex = 0;
   }
   if (m_IterBucketIndex == m_pTable->m_Bucket.size())
   {
      return false;
   }
   else
   {
      Key = m_pTable->m_Bucket[m_IterBucketIndex]->operator[](m_IterItemIndex)->Key;
      Value = m_pTable->m_Bucket[m_IterBucketIndex]->operator[](m_IterItemIndex)->Value;
      m_IterItemIndex++;
      return true;
   }
}

template<class PKey, class PValue>
bool LEGrefHashTableIterator<PKey,PValue>::iterateNext()
{
   if (m_IterBucketIndex == npos)
   {
      m_IterBucketIndex = 0;
   }
   while (m_IterBucketIndex < m_pTable->m_Bucket.size() && 
           m_pTable->m_Bucket[m_IterBucketIndex]->size() <= m_IterItemIndex )
   {
      m_IterBucketIndex++;
      m_IterItemIndex = 0;
   }
   if (m_IterBucketIndex == m_pTable->m_Bucket.size())
   {
      return false;
   }
   else
   {
      m_IterItemIndex++;
      return true;
   }
}

template<class PKey, class PValue>
const PKey& LEGrefHashTableIterator<PKey,PValue>::key()
{
   return m_pTable->m_Bucket[m_IterBucketIndex]->operator[](m_IterItemIndex-1)->Key;
}

template<class PKey, class PValue>
PValue& LEGrefHashTableIterator<PKey,PValue>::value()
{
   return m_pTable->m_Bucket[m_IterBucketIndex]->operator[](m_IterItemIndex-1)->Value;
}


template<class PKey, class PValue>
LEGrefHashTable<PKey, PValue>::LEGrefHashTable
(
   size_t CountOfBucket
) : m_Bucket(CountOfBucket)
{
   init(CountOfBucket);
}

template<class PKey, class PValue>
void LEGrefHashTable<PKey, PValue>::init(size_t CountOfBucket)
{
   removeAll();
   m_Size = 0;
   m_Bucket.resize(CountOfBucket);
   for(size_t BucketIndex=0; BucketIndex < m_Bucket.size(); ++BucketIndex)
   {
      m_Bucket[BucketIndex] = new LEGrefVect< LEGpair<PKey,PValue>* >(2, 0, true);
      // initial size of 0 grow by doubling.
   }
   //clear keys
   m_Keys.clear();
}

template<class PKey, class PValue>
LEGrefHashTable<PKey, PValue>::~LEGrefHashTable()
{
   try
   {
      removeAll();
   }
   catch (const COLerror& Error)
   {
      //COL_LOG(Error);
   }
}

template<class PKey, class PValue>
void LEGrefHashTable<PKey, PValue>::removeAll()
{
   for(size_t BucketIndex = 0;
       BucketIndex < m_Bucket.size();
       ++BucketIndex)
   {
      for(size_t ItemIndex = 0; ItemIndex < m_Bucket[BucketIndex]->size();
          ++ItemIndex)
      {
         delete m_Bucket[BucketIndex]->operator[](ItemIndex);
      }
      delete m_Bucket[BucketIndex];
   }
   m_Size = 0;
   m_Keys.clear(); //clear keys
}

template<class PKey, class PValue>
LEGpair<PKey,PValue>* LEGrefHashTable<PKey, PValue>::findPair(const PKey& Key) const
{
   size_t BucketIndex;
   size_t ItemIndex;
   findIndex(Key, BucketIndex, ItemIndex);

   if (ItemIndex == npos)
   {
      return NULL;
   }
   else
   {
      return m_Bucket[BucketIndex]->operator[](ItemIndex);
   }
}

template<class PKey, class PValue>
PValue& LEGrefHashTable<PKey, PValue>::operator[](const PKey& Key)
{
   LEGpair<PKey, PValue>* pPair = findPair(Key);

   if (!pPair)
   {
      insert(Key, PValue());
      pPair = findPair(Key);
   }
   COLPRECONDITION(pPair != NULL)
   return pPair->Value;
}

template<class PKey, class PValue>
const PValue& LEGrefHashTable<PKey, PValue>::operator[](const PKey& Key) const
{
   LEGpair<PKey, PValue>* pPair = findPair(Key);

   if (!pPair)
   {
      throw COLerror("Key does not exist.", COLerror::PreCondition);
   }

   return pPair->Value;
}


template<class PKey, class PValue>
bool LEGrefHashTable<PKey, PValue>::has(const PKey& Key) const
{
   LEGpair<PKey, PValue>* pPair = findPair(Key);
   if (!pPair)
      return false;
   else
      return true;
}

template<class PKey, class PValue>
PValue* LEGrefHashTable<PKey, PValue>::getValue(const PKey& Key) const
{
   LEGpair<PKey, PValue>* pPair = findPair(Key);
   if (pPair)
      return &pPair->Value;
   else
      return NULL;
}

template<class PKey, class PValue>
const PKey* LEGrefHashTable<PKey, PValue>::getKey(const size_t KeyIndex) const
{
  //simply pull from key vector
  return m_Keys[KeyIndex];
}

template<class PKey, class PValue>
void LEGrefHashTable<PKey, PValue>::clear()
{
   size_t CountOfBucket = m_Bucket.size();
   init(CountOfBucket);
}

template<class PKey, class PValue>
void LEGrefHashTable<PKey, PValue>::findIndex
(
   const PKey& Key,
   size_t& BucketIndex,
   size_t& ItemIndex
) const
{
   BucketIndex = LEGhashFunc(Key) % m_Bucket.size();
   ItemIndex = 0;
   while (ItemIndex < m_Bucket[BucketIndex]->size()
          && Key != m_Bucket[BucketIndex]->operator[](ItemIndex)->Key)
   {
      ItemIndex++;
   }
   if (m_Bucket[BucketIndex]->size() == ItemIndex)
   {
      ItemIndex = npos;
   }
}

template<class PKey, class PValue>
void LEGrefHashTable<PKey, PValue>::remove(const PKey& Key)
{
   size_t BucketIndex;
   size_t ItemIndex;
   findIndex(Key, BucketIndex, ItemIndex);

   if (ItemIndex == npos)
   {
      return;
   }
   else
   {
     //find and remove key
      size_t KeyIndex = 0;
      while (KeyIndex < m_Keys.size()
        && Key != *m_Keys.operator[](KeyIndex))
       {
          KeyIndex++;
       }
      m_Keys.remove(KeyIndex);

      delete m_Bucket[BucketIndex]->operator[](ItemIndex);
      m_Bucket[BucketIndex]->remove(ItemIndex);
      m_Size--;
   }
}

template<class PKey, class PValue>
void LEGrefHashTable<PKey,PValue>::insert(const PKey& Key, const PValue& Value)
{
   size_t BucketIndex;
   size_t ItemIndex;
   findIndex(Key, BucketIndex, ItemIndex);
   if (ItemIndex != npos)
   {
      m_Bucket[BucketIndex]->operator[](ItemIndex)->Value = Value;
   }
   else
   {
      m_Size +=1;
      LEGpair<PKey,PValue>* NewItem = new LEGpair<PKey,PValue>(Key,Value);
      m_Bucket[BucketIndex]->push_back(NewItem);
      m_Keys.push_back(&NewItem->Key); //insert key only if new
   }
}

// Simple template that allows for joining two smaller bit length types to a larger
// bit length. eg
//
// COLuint32 NewValue = LEGmakeBits<COLuint32,COLuint16>(0xaaaa,0xbbbb)
//
// Then NewValue will be 0xaaaabbbb
template< class FullType, class HalfType >
FullType LEGmakeBits( const HalfType HighValue, const HalfType LowValue )
{
   COLPRECONDITION(sizeof(FullType) == (sizeof(HalfType) + sizeof(HalfType))); 
   FullType NewValue = 0;
   COLuint8* pByte = (COLuint8*)&NewValue;
   COLuint8* pHighByte = (COLuint8*)&HighValue;
   COLuint8* pLowByte = (COLuint8*)&LowValue;
   //TODO change size_t to COLindex
   COLindex FullTypeSize = sizeof(FullType);
   COLindex HalfTypeSize = sizeof(HalfType);

   for( COLindex ByteIndex = 0; ByteIndex < FullTypeSize; ByteIndex++ )
   {
      if (ByteIndex < HalfTypeSize) //write low order
      {
         *pByte = *pLowByte;
         pByte++;
         pLowByte++;
      }
      else //write high order
      {
         *pByte = *pHighByte;
         pHighByte++;
         pByte++;
      }
   }

   return NewValue;
}

template< class FullType, class HalfType >
void LEGgetBitHalves( const FullType Value, HalfType& LowValueOut, HalfType& HighValueOut)
{
   COLPRECONDITION(sizeof(FullType) == (sizeof(HalfType) + sizeof(HalfType))); 
   HalfType LowValue = 0;
   HalfType HighValue = 0;
   COLuint8* pByte = (COLuint8*)&Value;
   COLuint8* pHighByte = (COLuint8*)&HighValue;
   COLuint8* pLowByte = (COLuint8*)&LowValue;
   COLindex FullTypeSize = sizeof(FullType);
   COLindex HalfTypeSize = sizeof(HalfType);

   for(COLindex ByteIndex = 0; ByteIndex < FullTypeSize; ByteIndex++ )
   {
      if (ByteIndex >= HalfTypeSize) //get high order
      {
         *pHighByte = *pByte;
         pByte++;
         pHighByte++;
      }
      else //get low order
      {
         *pLowByte = *pByte;
         pLowByte++;
         pByte++;
      }
   }
   LowValueOut = LowValue;
   HighValueOut = HighValue;
}

#endif // end of defensive include
