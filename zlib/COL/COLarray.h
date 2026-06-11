#ifndef __COL_ARRAY_H__
#define __COL_ARRAY_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLarray
//
// Description: Originally called COLarray. Dynamic vector class patterned after STL vector. Worth reading through it.
//
// The objects stored in the template must support
//
//   Type::Type()
//   Type::Type(const Type&)
//
// Authors: Nasron Cheong, Andrew Vajoczki
// Date:   Dec 2008
//---------------------------------------------------------------------------

// Define to preserve the legacy COLarray legacy behavior to use memmove to
// move elements in the vector rather rather invoking the copy constructor
// and in-place destructor. Some parser classes depend on this COLarray
// element 'sliding' behavior since they either do not have copy
// constructors, or they have errors in these functions.
//
// The legacy memmove behavior limits the types of objects that can be 
// stored in COLarrays to ones whose bit patterns can safely be shifted.
// It also is confusing in that the address of the object in the constructor
// can differ from the address in the destructor because the element
// may have been shifted without first invoking the copy ctor and dtor.
//
// When all such classes are fixed, this behavior can be disabled.
//
// The legacy memmove behaviour has been temporarily commented out to see if
// this causes any problems. Once enough time has passed to consider it fine,
// the macro definition and related code blocks can be removed. See #22600 for
// more info.
//#define COLarray_LEGACY_MEMMOVE_BEHAVIOR

// Uncomment this to enable extra debugging for destructed elements.
//#define COLarray_MEMSET_DESTRUCTED

#include "COLerror.h"
#include <new>

#ifdef COLarray_MEMSET_DESTRUCTED
#  include <memory.h>
#  define COLarray_CLEAR_ELEMENT_MEMORY(m_) memset((m_),0xd7,sizeof(*(m_)))
#else
#  define COLarray_CLEAR_ELEMENT_MEMORY(m_) 
#endif

template <typename T> class COLarray{
public:
   typedef T        value_type;
   typedef T&       reference;
   typedef const T& const_reference;
   typedef T*       pointer;
   typedef const T* const_pointer;
   typedef int      size_type; // MUST be a signed type
   typedef T*       iterator;
   typedef const T* const_iterator;

   typedef T TValue;
   typedef T TContained;

   COLarray(){
      size_ = 0;
      capacity_ = 0;
      heap_ = 0;
   }

   explicit COLarray(size_type n, const T& v = T()){
      if (n<0) n = 0;
      size_ = 0;
      capacity_ = 0;
      heap_ = 0;
      setCapacity(n);
      for (size_type i = n; i--;){
         new ((void*)&heap_[i]) T(v);
      }
      size_ = n;
   }

   COLarray(const COLarray& that) : size_(0), capacity_(0){
      heap_ = 0;
      *this = that;
   }

   ~COLarray(){
      clear();
   }

   COLarray& operator=(const COLarray& that){
      if (this != &that){
         clear();
         size_type N = that.size_;
         setCapacity(N);
         for (size_type i = 0; i < N; i++){
            push_back(that.heap_[i]);
         }
      }
      return *this;
   }

   size_type capacity() const { return capacity_; }
   size_type size() const     { return size_; }
   iterator begin()           { return &heap_[0]; }
   const_iterator begin() const { return &heap_[0]; }

   iterator end() { return &heap_[size_]; }

   const_iterator end() const { return &heap_[size_];}

   T& push_back(const T& Value){
      setCapacity(size_ + 1);
      new ((void*)&heap_[size_]) T(Value);
      return heap_[size_++];
   }

   T& push_back(){
      setCapacity(size_ + 1);
      new ((void*)&heap_[size_]) T();
      return heap_[size_++];
   }

   void pop_back(){
      if (size_ > 0){
         erase(&heap_[size_ - 1]);
      }
   }

   T& operator[](size_type n){
      COLPRECONDITION(n >= 0 && n < size_);
      return heap_[n];
   }

   const T& operator[](size_type n) const{
      COLPRECONDITION(n >= 0 && n < size_);
      return heap_[n];
   }

   void clear(){
      for (size_type i = size_; i--;){
         heap_[i].~T();
         COLarray_CLEAR_ELEMENT_MEMORY(&heap_[i]);
      }
      deleteHeap();
      capacity_ = 0;
      size_ = 0;
   }

   iterator erase(iterator pos){
      iterator E = end();
      if (begin() <= pos && pos < E){
#ifdef COLarray_LEGACY_MEMMOVE_BEHAVIOR
         pos->~T();
         COLarray_CLEAR_ELEMENT_MEMORY(pos);
         memmove((void*)pos, pos+1, (E-pos-1)*sizeof(T));
#else
         for (iterator p = pos; p < E;){
            p->~T();
            COLarray_CLEAR_ELEMENT_MEMORY(p);
            p++;
            if (p < E){
               new ((void*)(p-1)) T(*p);
            }
         }
#endif
         --size_;
      }
      return pos;
   }

   void setCapacity(size_type capacityRequested){
      if (capacityRequested <= 0 || capacityRequested <= capacity_) return;
      size_type biggerCapacity = capacity_ * 2;
      if (biggerCapacity > capacityRequested){
         capacityRequested = biggerCapacity;
      }
      const size_type minimum_capacity = 8;
      if (capacityRequested < minimum_capacity){
         capacityRequested = minimum_capacity;
      }
      T* newElements = (T*) new char [sizeof(T) * capacityRequested];
#ifdef COLarray_LEGACY_MEMMOVE_BEHAVIOR
      memcpy((void*)newElements, heap_, size_*sizeof(T));
#else
      for (size_type i = size_; i--;){
         new ((void*)&newElements[i]) T(heap_[i]);
         heap_[i].~T();
         COLarray_CLEAR_ELEMENT_MEMORY(&heap_[i]);
      }
#endif
      deleteHeap();
      heap_ = newElements;
      capacity_ = capacityRequested;
   }

   void remove(size_type ItemIndex){
      COLPRECONDITION(ItemIndex >= 0 && ItemIndex < size_);
      erase(&heap_[ItemIndex]);
   }

   void resize(size_type newSize){
      if (newSize == 0){
         clear();
         COLPRECONDITION(newSize == size_);
         return;
      }
      size_type originalSize = size_;
      if (newSize < originalSize){
         // shrink vector and destroy elements
         for (size_type i = originalSize - newSize; i > 0; --i){
            pop_back();
         }
      } else {
         // newSize >= originalSize
         // grow vector and append elements
         setCapacity(newSize);
         for (size_type i = newSize - originalSize; i > 0; --i){
            push_back();
         }
      }
      COLPRECONDITION(newSize == size_);
   }

   void insert(const T& Value, size_type ItemIndex){
      COLPRECONDITION(ItemIndex >= 0 && ItemIndex <= size_);
      if (ItemIndex >= 0 && ItemIndex <= size_) {
         size_type originalSize = size_;
         setCapacity(originalSize + 1);
         // move the objects over until we hit ItemIndex slot
#ifdef COLarray_LEGACY_MEMMOVE_BEHAVIOR
         memmove((void*)&heap_[ItemIndex+1], &heap_[ItemIndex], (size_-ItemIndex)*sizeof(T));
#else
         for (size_type i = originalSize - 1; i >= ItemIndex; --i) {
            new ((void*)&heap_[i+1]) T(heap_[i]);
            heap_[i].~T();
            COLarray_CLEAR_ELEMENT_MEMORY(&heap_[i]);
         }
#endif
         // drop in the new item
         new ((void*)&heap_[ItemIndex]) T(Value);
      }
      ++size_;
   }

   void insert(size_type ItemIndex){
      insert(T(), ItemIndex);
   }

   T& back(){
      COLPRECONDITION(size_ > 0);
      return heap_[size_ - 1];
   }

   const T& back() const{
      COLPRECONDITION(size_ > 0);
      return heap_[size_ - 1];
   }

   T& front(){
      COLPRECONDITION(size_ > 0);
      return heap_[0];
   }

   const T& front() const{
      COLPRECONDITION(size_ > 0);
      return heap_[0];
   }

   T* getArray() { return heap_;}

   const T* getArray() const { return heap_;}

   size_type indexOf(const T& Value) const{
      for (size_type i = 0; i < size_; i++){
         if (heap_[i] == Value){
            return i;
         }
      }
      return -1;
   }

private:
   void deleteHeap(){
      delete [] (char*)heap_;
      heap_ = 0;
   }

   size_type size_;
   size_type capacity_;
   T* heap_;
};

#endif // end of defensive include

