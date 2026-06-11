#ifndef __LEG_REF_VECT_H__
#define __LEG_REF_VECT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: LEGrefVect
//
// Description:
//
// THIS IS LEGACY NOW.  New code should use COLarray and we will gradually phase
// this class out.
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
// Notice the usage of the LEGvoidVect.  This is used in support of the thin
// template pattern for faster compile time.  See intranet for more details.
//
// Author: Eliot Muir
// Date:   Tue 08/25/1998 
//
//---------------------------------------------------------------------------

#include <COL/COLerror.h>


//#define VOID_PARENT
//#define WITH_COPY_CONSTRUCTOR

#ifdef VOID_PARENT

class LEGvoidVect
{
public:
   LEGvoidVect(size_t GrowFactor = 2,
             size_t InitialSize=0,
             bool GrowByDouble=true);
   virtual ~LEGvoidVect();

   void* operator[](size_t ItemIndex);
   const void* operator[](size_t ItemIndex) const;

   void insert(void* pValue, size_t ItemIndex);

   void remove(size_t ItemIndex);

   size_t size() const;

   size_t capacity() const;

   void resize(size_t NewSize);

   void clear();
   void fullClear();

   void* push_back(void* Value);
   void pop_back();
   void* back();
   //
protected:
   virtual void destroyItem(void* pItem);
   virtual void copyItem(void*& pNewItem, const void* pOldItem);

   LEGvoidVect& copy(const LEGvoidVect& Orig);
   LEGvoidVect& operator=(const LEGvoidVect& Original);
   LEGvoidVect(const LEGvoidVect& Original)
   {
      operator=(Original);
   }
private:
   size_t m_GrowBy;
   size_t m_Size;
   size_t m_Capacity;
//   size_t m_InitialCapacity;

   bool m_GrowByDouble;

   void** m_pData;

   void grow(size_t RequiredSize);

   
};

//LEGACY - don't use.
template<class Item>
class LEGrefVect : public LEGvoidVect
{
public:
   //LEGACY - don't use.
   LEGrefVect(size_t GrowFactor = 2,
              size_t InitialSize=0,
              bool GrowByDouble=true)
    : LEGvoidVect(GrowFactor, InitialSize, GrowByDouble) {}

   Item& operator[](size_t ItemIndex) 
   { 
      return (*(Item*)LEGvoidVect::operator[](ItemIndex));
   }
   const Item& operator[](size_t ItemIndex) const
   { 
      return (*(Item*)LEGvoidVect::operator[](ItemIndex));
   }

   void insert(const Item& Value, size_t ItemIndex)
   {
#ifdef WITH_COPY_CONSTRUCTOR
      LEGvoidVect::insert((void*)new Item(Value), ItemIndex);
#else
      Item* pNewItem = new Item();
      *pNewItem = Value;
      LEGvoidVect::insert((void*)pNewItem, ItemIndex);
#endif
   }

   Item& push_back(const Item& Value)
   {
#ifdef WITH_COPY_CONSTRUCTOR
      return *((Item*)LEGvoidVect::push_back((void*)new Item(Value)));
#else
      Item* pNewItem = new Item();
      *pNewItem = Value;
      LEGvoidVect::push_back((void*)pNewItem);
      return *pNewItem;
#endif
   }
    
   //Item& back()
   //{
   //   return *((Item*)LEGvoidVect::back());
   //}

   LEGrefVect<Item>& operator=(const LEGrefVect<Item>& Original)
   {      
      return *((LEGrefVect<Item>*)&LEGvoidVect::copy(Original));
   }
   LEGrefVect<Item>(const LEGrefVect<Item>& Original)
   {
      operator=(Original);
   }
protected:
   virtual void copyItem(void*& pNewItem, const void* pOldItem)
   {
#ifdef WITH_COPY_CONSTRUCTOR
      pNewItem = (void*)new Item(*((const Item*)pOldItem));
#else
      Item* pItem = new Item();
      *pItem = *((Item*)pOldItem);
      pNewItem = (void*)pItem;
#endif
   }

   virtual void destroyItem(void* pItem)
   {
      delete ((Item*)pItem);
   }
private:   
};

#else

template<class Item>
class LEGrefVect 
{
public:
   LEGrefVect(size_t GrowFactor = 2,
             size_t InitialSize=0,
             bool GrowByDouble=true);
   ~LEGrefVect();

   Item& operator[](size_t ItemIndex);
   const Item& operator[](size_t ItemIndex) const;

   void insert(const Item& Value, size_t ItemIndex);

   void remove(size_t ItemIndex);

   size_t size() const;

   size_t capacity() const { return m_Capacity; }

   void resize(size_t NewSize);

   void clear();
   void fullClear();

   Item& push_back(const Item& Value);
   Item& pop_back();
   Item& back();
   LEGrefVect<Item>& operator=(const LEGrefVect<Item>& Original);
   LEGrefVect(const LEGrefVect<Item>& Original) { operator=(Original); }

protected:
   virtual void itemCopy( Item& ValueNew, Item& ValueOld ){ ValueNew = ValueOld; }

private:
//   size_t m_GrowBy; we no longer use these
   size_t m_Size;
   size_t m_Capacity;

//   bool m_GrowByDouble;

   Item* m_pData;

   void grow(size_t RequiredSize);

};

template<class Item>
LEGrefVect<Item>::LEGrefVect
(
   size_t GrowBy,
   size_t InitialSize,
   bool GrowByDouble
) : m_Size(0),
    m_Capacity(InitialSize)
//m_GrowBy(GrowBy),
//    m_GrowByDouble(GrowByDouble)
{
//   COLPRECONDITION(GrowBy > 0);
   if (m_Capacity > 0)
   {
      m_pData = new Item[m_Capacity];
   }
   else
   {
      m_pData = NULL;
   }
}

template<class Item>
LEGrefVect<Item>::~LEGrefVect()
{
   delete []m_pData;
}

template<class Item>
void LEGrefVect<Item>::clear()
{
   m_Size = 0;
}

template<class Item>
void LEGrefVect<Item>::fullClear()
{
   if( !m_Capacity )
   {
      m_Capacity = 1;
   }
   delete []m_pData;
   m_pData = new Item[m_Capacity];
   m_Size =0;
}

template<class Item>
Item& LEGrefVect<Item>::operator[](size_t ItemIndex)
{
   COLPRECONDITION(ItemIndex < m_Size);
   return m_pData[ItemIndex];
}

template<class Item>
const Item& LEGrefVect<Item>::operator[](size_t ItemIndex) const
{
   COLPRECONDITION(ItemIndex < m_Size);
   return m_pData[ItemIndex];
}

template<class Item>
size_t LEGrefVect<Item>::size() const
{
   return m_Size;
}

template<class Item>
void LEGrefVect<Item>::resize(size_t NewSize)
{
   while( NewSize < m_Size )//allow shrinking
   {
      m_Size--;
      m_pData[m_Size] = Item();  // necessary to call destructor...
   }
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

   //Item* pNewData = new Item[NewSize];
   //size_t ItemIndex;
   //for(ItemIndex = 0;
   //    ItemIndex < m_Size;
   //    ItemIndex++)
   //{
   //   pNewData[ItemIndex] = m_pData[ItemIndex];
   //}
   //for(ItemIndex = m_Size;
   //    ItemIndex < NewSize;
   //    ItemIndex++)
   //{
   //    pNewData[ItemIndex] = Item();  // initialise empty space
   //}
   //delete []m_pData;
   //m_pData = pNewData;
   //m_Size = NewSize;
   //m_Capacity = NewSize;
}

template<class Item>
void LEGrefVect<Item>::remove(size_t iItemIndex)
{
   COLPRECONDITION((iItemIndex < m_Size) && (m_Size != 0));
   for(size_t ItemIndex = iItemIndex;
       ItemIndex < m_Size-1;
       ++ItemIndex)
   {
      itemCopy( m_pData[ItemIndex], m_pData[ItemIndex+1] );
   }
   m_Size--;
   m_pData[m_Size] = Item();  // necessary to call destructor...
}

template<class Item>
void LEGrefVect<Item>::insert(const Item& Value, size_t ItemIndex)
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
      itemCopy( m_pData[CopyIndex], m_pData[CopyIndex-1] );
   }
   m_pData[ItemIndex] = Value;
   m_Size++;
}

template<class Item>
Item& LEGrefVect<Item>::push_back(const Item& Value)
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

template<class Item>
Item& LEGrefVect<Item>::pop_back()
{
   COLPRECONDITION(m_Size > 0);
   m_Size--;
   return m_pData[m_Size];
}

template<class Item>
Item& LEGrefVect<Item>::back()
{
   COLPRECONDITION(m_Size > 0);
   return m_pData[m_Size-1];
}

size_t COL_DLL_FUNC(LEGrefVectResizeFunc)(size_t CurrentSize,size_t RequiredSize);

template<class Item>
void LEGrefVect<Item>::grow(size_t RequiredSize)
{
   COLPRECONDITION(RequiredSize > 0);
   size_t NewCapacity = LEGrefVectResizeFunc(m_Capacity,RequiredSize);
   COLPRECONDITION(NewCapacity >= RequiredSize);

//   COLcout << "Vector growing to " << NewCapacity
//            << " from " << m_Capacity);
   Item* pNewData = new Item[NewCapacity];
   size_t ItemIndex;
   for(ItemIndex = 0;
       ItemIndex < m_Size;
       ItemIndex++)
   {
      itemCopy( pNewData[ItemIndex], m_pData[ItemIndex] );
   }
   // probably don't need this code...
   //for (ItemIndex = m_Size; ItemIndex < m_Capacity; ++ItemIndex)
   //{
   //   pNewData[ItemIndex] = Item();
   //}
   delete []m_pData;
   m_pData = pNewData;
   m_Capacity = NewCapacity;
}

template<class Item>
LEGrefVect<Item>& LEGrefVect<Item>::operator=(const LEGrefVect<Item>& Original)
{
//   m_GrowBy = Original.m_GrowBy;
   m_Size   = Original.m_Size;
   m_Capacity = Original.m_Capacity;
//   m_GrowByDouble = Original.m_GrowByDouble;
   COLPRECONDITION(m_Size <= m_Capacity);
   delete []m_pData;
   m_pData = new Item[m_Capacity];
   for (size_t DataIndex = 0; DataIndex < m_Size; ++DataIndex)
   {
      m_pData[DataIndex] = Original.m_pData[DataIndex];
   }
   return *this; 
}

#endif // no_parent

#endif // end of defensive include
