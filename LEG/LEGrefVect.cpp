//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LEGvoidVect
//
// Description:
//
// Deprecated Junk
//
// Simple templated reference vector class
//
// The objects stored in the template must support 
// Item& operator=(const Item&)
//
// By default the vector will grow in chunks of 10 - however the grow factor
// can be altered or made into a product - i.e. growing by a multiple of the
// grow factor rather than a fixed step each other.  Growing is expensive -
// time will be saved if the vector can be pre-sized.
//
// Author: Eliot Muir
// Date:   Tue 08/25/1998 
//
//
//---------------------------------------------------------------------------

#include "LEGprecomp.h"
#pragma hdrstop

#include <LEG/LEGrefVect.h>

#ifdef VOID_PARENT

LEGvoidVect::LEGvoidVect
(
   size_t GrowBy,
   size_t InitialSize,
   bool GrowByDouble
) : m_GrowBy(GrowBy),
    m_Size(0),
  //  m_InitialCapacity(InitialSize),
    m_Capacity(0),
    m_GrowByDouble(GrowByDouble),
    m_pData(NULL)
{
   COLPRECONDITION(GrowBy > 0);
}

LEGvoidVect::~LEGvoidVect()
{
   try
   {
      clear();
   }
   catch(const COLerror& Error)
   {
      COL_LOG(Error);
   }
}

void LEGvoidVect::destroyItem(void* pItem)
{
   // base does nothing
}

void LEGvoidVect::clear()
{
   for (size_t ItemIndex=0;
        ItemIndex < m_Size;
        ItemIndex++)
   {
      COLPRECONDITION(m_pData != NULL);
      destroyItem(m_pData[ItemIndex]);
   }
   delete []m_pData;
   m_pData = NULL;
}

void LEGvoidVect::fullClear()
{
   clear();
}

void* LEGvoidVect::operator[](size_t ItemIndex)
{
   COLPRECONDITION(ItemIndex < m_Size);
   return m_pData[ItemIndex];
}

const void* LEGvoidVect::operator[](size_t ItemIndex) const
{
   COLPRECONDITION(ItemIndex < m_Size);
   return m_pData[ItemIndex];
}

size_t LEGvoidVect::size() const
{
   return m_Size;
}

void LEGvoidVect::resize(size_t NewSize)
{
   // TODO ??? is this going to work??
   COLPRECONDITION(NewSize >= m_Size);
   if (NewSize == m_Size)
   {
      // no need to resize...
      return;
   }
   if (m_Capacity < NewSize)
   {
      grow(NewSize);
   }
   m_Size = NewSize;
}


void LEGvoidVect::remove(size_t iItemIndex)
{
   COLPRECONDITION((iItemIndex < m_Size) && (m_Size != 0));
   destroyItem(m_pData[iItemIndex]);
   for(size_t ItemIndex = iItemIndex;
       ItemIndex < m_Size-1;
       ++ItemIndex)
   {
      m_pData[ItemIndex] = m_pData[ItemIndex+1];
   }
   m_Size--;
}


void LEGvoidVect::insert(void* Value, size_t ItemIndex)
{
   COLPRECONDITION(ItemIndex <= m_Size)
   if (m_Size == m_Capacity)
   {
       grow(m_Capacity+1);
   }
   COLPOSTCONDITION(m_Size < m_Capacity);

   for (size_t CopyIndex = m_Size;
        CopyIndex > ItemIndex;
        --CopyIndex)
   {
      m_pData[CopyIndex] = m_pData[CopyIndex-1];
   }
   m_pData[ItemIndex] = Value;
   m_Size++;
}

void* LEGvoidVect::push_back(void* Value)
{
   if (m_Size == m_Capacity)
   {
      grow(m_Capacity+1);
   }
   COLPOSTCONDITION(m_Size < m_Capacity);

   m_pData[m_Size] = Value;
   m_Size++;
   return m_pData[m_Size-1];
}

void LEGvoidVect::pop_back()
{
   COLPRECONDITION(m_Size > 0);
   m_Size--;
   destroyItem(m_pData[m_Size]);
}

void* LEGvoidVect::back()
{
   COLPRECONDITION(m_Size > 0);
   return m_pData[m_Size-1];
}

void LEGvoidVect::grow(size_t RequiredSize)
{
   COLPRECONDITION(RequiredSize > 0);
   COLPRECONDITION(m_GrowBy > 0);
   size_t NewCapacity(m_Capacity);
   if (m_GrowByDouble)
   {
      if (NewCapacity == 0)
      {
         NewCapacity = 1;
      }
      while (NewCapacity < RequiredSize)
      {
         NewCapacity = m_GrowBy * NewCapacity;
      }
   }
   else
   {
      while (NewCapacity < RequiredSize)
      {
         NewCapacity = m_GrowBy + NewCapacity;
      }
   }

   void** pNewData = new void*[NewCapacity];
   size_t ItemIndex;
   for(ItemIndex = 0;
       ItemIndex < m_Size;
       ItemIndex++)
   {
      pNewData[ItemIndex] = m_pData[ItemIndex];
   }
   delete []m_pData;
   m_pData = pNewData;
   m_Capacity = NewCapacity;
}

LEGvoidVect& LEGvoidVect::copy(const LEGvoidVect& Original)
{
   m_GrowBy = Original.m_GrowBy;
   m_Size   = Original.m_Size;
   m_Capacity = Original.m_Capacity;
   m_GrowByDouble = Original.m_GrowByDouble;
   COLPRECONDITION(m_Size <= m_Capacity);
   clear();
   m_pData = new void*[m_Capacity];
   for (size_t DataIndex = 0; DataIndex < m_Size; ++DataIndex)
   {
      copyItem(m_pData[DataIndex],Original.m_pData[DataIndex]);
   }
   return *this;
}

void LEGvoidVect::copyItem(void*& pNewItem, const void* pOldItem)
{
   pNewItem = (void*)pOldItem;
}

LEGvoidVect& LEGvoidVect::operator=(const LEGvoidVect& Original)
{
   copy(Original);
   return *this;
}

#endif  // VOID_PARENT
