#ifndef __COL_HASHMAP_H__
#define __COL_HASHMAP_H__
//-----------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:      COLhashmap
//
// Description:
//
// Very much worth having a read through this critical data-structure - Eliot.
//
// Unordered templated key/value hash map collection class with unique keys.
//
// COLhashmap is patterned after the STL unordered_map class.
//
//    COLhashmap<COLstring,int> MyMap;
//
//    MyMap["foo"] = 40;
//    MyMap["bar"] += 55;
//    MyMap["bar"] = 123;
//    MyMap["baz"] += 11;
//    MyMap["foo"] -= 333;
//    MyMap["bar"] -= 200;
//    MyMap["baz"] += 99;
//
//    // output will be unordered
//    for (auto it=MyMap.begin(), auto E= MyMap.end(); it != E; ++it) {
//       COLcout << "key = " << it->first
//               << ", value = " << it->second << newline;
//    }
//    if (int Count = MyMap.count("foo")) {
//       COLcout << "element foo exists with count of " << Count << newline;
//    }
//    auto ci = MyMap.find("bar");
//    if (ci != MyMap.cend()) {
//       COLcout << "Found item with key = " << ci->first
//               << ", value = " << ci->second << newline;
//    }
//
// Output:
//
//    key = bar, value = -77
//    key = foo, value = -293
//    key = baz, value = 110
//    element foo exists with count of 1
//    Found item with key = bar, value = -77
//
// COLhashmap also supports legacy COLhashmapPlace iteration which is
// deprecated and discouraged for new code.
//
// See http://www.cplusplus.com/reference/unordered_map/unordered_map/
//
// A custom hash function can be supplied to the class.
// It is defaulted for common key types like COLstring and numbers.
//
// When a collection is constructed it is passed a hash function that
// returns a COLindex.
//
// COLindex HashProc(const KeyType& Key);
//
// The operator == is used to test that keys match.
//
// The objects stored in the template must support the copy contructor
// Type(const Type&)
//
// A copy is made of an item when it is inserted into the list. When
// an item is removed from the list it is destructed.
//
// Author: Ian Farquharson
// Date:   02/05/2004
//
// Author: Andrew Vajoczki - added STL style methods and iterators.
//-----------------------------------------------------------------------------

#include <COL/COLerror.h>
#include <COL/COLhash.h>
#include <COL/COLpair.h>

class COLhashmapBaseNode{
public:
   COLhashmapBaseNode* Next;
};

typedef COLhashmapBaseNode* COLhashmapPlace;

class COLvoidLookup{
private:
   COLhashmapPlace*   Buckets;
   const COLindex*   pCountOfBucket;
   COLindex          Size;
   void init(void);
   inline void grow(void);
public:
   COLvoidLookup(void) { init(); }
   virtual ~COLvoidLookup(void);
   COLindex size(void) const { return Size; }
   void clear(void);
   void remove(COLhashmapPlace Place);
   COLindex countOfBucket() const;
   COLindex countOfBucketItem(COLindex BucketIndex) const;
   COLhashmapPlace first(void) const;
   COLhashmapPlace last(void) const;
   COLhashmapPlace previous(COLhashmapPlace Place) const;
   COLhashmapPlace next(COLhashmapPlace Place) const;
protected:
   bool removeItem(COLindex Hash, const void* Key);
   COLhashmapPlace addItem(COLindex Hash, const void* Key, COLhashmapPlace Node);
   COLhashmapPlace findItem(COLindex Hash, const void* Key) const;
protected:
   virtual void destroyItem(COLhashmapPlace Place) = 0;
   virtual COLindex hashItem(COLhashmapPlace Place) const = 0;
   virtual bool equalKey(const void* Key, COLhashmapPlace Place) const = 0;
private:
   COLvoidLookup(const COLvoidLookup& Lookup);               // disable copy constructor
   COLvoidLookup& operator =(const COLvoidLookup& Lookup);   // disable assignment
#ifdef COL_ENABLE_INVARIANT_INTEGRITY_CHECK
   void invariant(void) const;
#else
   void invariant(void) const {}
#endif
};

template <class KeyType, class Type>
struct COLhashmapNode: public COLhashmapBaseNode{
   COLindex Hash;
   KeyType  Key;
   Type     Item;
   COLhashmapNode(COLindex iHash, const KeyType& iKey, const Type& iItem)
   : Hash(iHash), Key(iKey), Item(iItem) {}
};

template <class KeyType, class Type, class CompareType = COLhash<KeyType> >
class COLhashmap: public COLvoidLookup{
public:
   struct iterator {
      iterator() : map_(NULL), place_(NULL) { }
      iterator(const iterator& that) : map_(that.map_), place_(that.place_) { }
      iterator& operator=(const iterator& that) {
         map_ = that.map_;
         place_ = that.place_;
         return *this;
      }
      iterator& operator++() {
         COLPRECONDITION(place_);
         place_ = map_->next(place_);
         return *this;
      }
      iterator operator++(int) {
         iterator tmp(*this);
         ++*this;
         return tmp;
      }
      iterator& operator--() {
         COLPRECONDITION(place_);
         place_ = map_->previous(place_);
         return *this;
      }
      iterator operator--(int) {
         iterator tmp(*this);
         --*this;
         return tmp;
      }
      bool operator==(const iterator& that) const {
         return that.map_   == this->map_
             && that.place_ == this->place_;
      }
      bool operator!=(const iterator& that) const {
         return !operator==(that);
      }
      // non-standard method to support (*iter).first, (*iter).second syntax
      COLpair<const KeyType&,Type&> operator*() {
         return COLpair<const KeyType&,Type&>(this->key(), this->value());
      }
      // non-standard method to support iter->first, iter->second syntax
      COLpair<const KeyType&,Type&> operator->() {
         return COLpair<const KeyType&,Type&>(this->key(), this->value());
      }
      //
      // non-STL methods
      //
      operator COLhashmapPlace() const { return place_; } // for legacy find()
      const KeyType& key() const { return map_->key(place_); }
      Type& value() { return map_->operator[](place_); }
      //
      // not public interface - treat as private
      //
      iterator(COLhashmap<KeyType,Type,CompareType>* m, COLhashmapPlace p) : map_(m), place_(p) { }
      COLhashmap<KeyType,Type,CompareType>* map_;
      COLhashmapPlace place_;
   };

   struct const_iterator {
      const_iterator() : map_(NULL), place_(NULL) { }
      const_iterator(const const_iterator& that) : map_(that.map_), place_(that.place_) { }
      const_iterator(const iterator& that) : map_(that.map_), place_(that.place_) { }
      const_iterator& operator=(const const_iterator& that) {
         map_ = that.map_;
         place_ = that.place_;
         return *this;
      }
      const_iterator& operator=(const iterator& that) {
         map_ = that.map_;
         place_ = that.place_;
         return *this;
      }
      const_iterator& operator++() {
         COLPRECONDITION(place_);
         place_ = map_->next(place_);
         return *this;
      }
      const_iterator operator++(int) {
         const_iterator tmp(*this);
         ++*this;
         return tmp;
      }
      const_iterator& operator--() {
         COLPRECONDITION(place_);
         place_ = map_->previous(place_);
         return *this;
      }
      const_iterator operator--(int) {
         const_iterator tmp(*this);
         --*this;
         return tmp;
      }
      bool operator==(const const_iterator& that) const {
         return that.map_   == this->map_
             && that.place_ == this->place_;
      }
      bool operator!=(const const_iterator& that) const {
         return !operator==(that);
      }
      // non-standard method to support (*iter).first, (*iter).second syntax
      COLpair<const KeyType&,const Type&> operator*() const {
         return COLpair<const KeyType&,const Type&>(this->key(), this->value());
      }
      // non-standard method to support iter->first, iter->second syntax
      const COLpair<const KeyType&,const Type&> operator->() const {
         return COLpair<const KeyType&,const Type&>(this->key(), this->value());
      }
      //
      // non-STL methods
      //
      operator COLhashmapPlace() const { return place_; } // for legacy find()
      const KeyType& key() const { return map_->key(place_); }
      const Type& value() const { return map_->operator[](place_); }
      //
      // not public interface - treat as private
      //
      const_iterator(const COLhashmap<KeyType,Type,CompareType>* m, COLhashmapPlace p) : map_(m), place_(p) { }
      const COLhashmap<KeyType,Type,CompareType>* map_;
      COLhashmapPlace place_;
   };

   bool empty() const { return 0 == this->size(); }

   iterator       begin()        { return iterator(this, this->first()); }
   const_iterator begin()  const { return const_iterator(this, this->first()); }
   const_iterator cbegin() const { return const_iterator(this, this->first()); }

   iterator       end()          { return iterator(this, NULL); }
   const_iterator end()  const   { return const_iterator(this, NULL); }
   const_iterator cend() const   { return const_iterator(this, NULL); }

   // determine whether key exists
   int count(const KeyType& Key) const {
      return this->find(Key) == this->end() ? 0 : 1;
   }

   iterator erase(iterator position) {
      COLASSERT(this == position.map_); // iterator must match map instance
      if (position.place_) {
         COLhashmapPlace Next = this->next(position.place_);
         this->remove(position.place_);
         return iterator(this, Next);
      }
      // it is not an error to erase(end())
      return this->end();
   }

   iterator erase(const KeyType& key) {
      return this->erase(this->find(key));
   }

   //public typedefs for generic functions
   typedef KeyType         TKey;
   typedef Type            TValue;
   typedef COLhashmapPlace TPlace;

   typedef COLindex (*HashProc)(const KeyType& Key);

   COLhashmap(): Hash(CompareType::defaultHash) {}
   COLhashmap(HashProc TheHashProc): Hash(TheHashProc) {}

   virtual ~COLhashmap(void) { 
      try {
         clear(); 
      }
      catch (const COLerror& Error) {
         // There was an error logged here...
      }
   }

   COLhashmap(const COLhashmap& Orig) : Hash(Orig.Hash) {
      *this = Orig;
   }

   COLhashmap& operator=(const COLhashmap&);

   Type& operator[](COLhashmapPlace Place) {
      COLPRECONDITION(Place != NULL);
      return ((COLhashmapNode<KeyType, Type>*) Place)->Item;
   }

   const Type& operator[](COLhashmapPlace Place) const {
      COLPRECONDITION(Place != NULL);
      return ((COLhashmapNode<KeyType, Type>*) Place)->Item;
   }
   
   KeyType& key(COLhashmapPlace Place) {
      COLPRECONDITION(Place != NULL);
      return ((COLhashmapNode<KeyType, Type>*) Place)->Key;
   }

   const KeyType& key(COLhashmapPlace Place) const {
      COLPRECONDITION(Place != NULL);
      return ((COLhashmapNode<KeyType, Type>*) Place)->Key;
   }

   Type& operator[](const KeyType& Key);
   const Type& operator[](const KeyType& Key) const;

   bool removeKey(const KeyType& Key) { return removeItem(Hash(Key), &Key); }

   COLhashmapPlace add(const KeyType& Key, const Type& Item);

   void emplace(const KeyType& Key, const Type& Item) {
      this->add(Key, Item);
   }

   // legacy COLhashmapPlace find(const KeyType& Key) const replaced with iterator versions.
   // iterator classes have an operator COLhashmapPlace() method for backwards compatibility.
   const_iterator find(const KeyType& Key) const {
      return const_iterator(this, findItem(Hash(Key),&Key));
   }
   iterator find(const KeyType& Key) {
      return iterator(this, findItem(Hash(Key),&Key));
   }

   template<class TClass>
   void iterate(void (TClass::*pMethodVoid)(const KeyType&,Type&),TClass* pInstance, COLhashmapPlace From = NULL, COLhashmapPlace To = NULL){
      if (!From) {
         From = this->first();
      }
      while (From != NULL) {
         (pInstance->*pMethodVoid)(this->key(From),this->operator[](From));
         if (To && To == From) {
            break;
         }
         From = this->next(From);
      }
   }

   template<class TClass>
   COLhashmapPlace iterate(COLhashmapPlace (TClass::*pMethodVoid)(const KeyType&,Type&,COLhashmapPlace CurrentPlace),TClass* pInstance, COLhashmapPlace From=NULL, COLhashmapPlace To=NULL){
      if (!From) {
         From = this->first();
      }
      while (From != NULL) {
         COLhashmapPlace Found = (pInstance->*pMethodVoid)(this->key(From),this->operator[](From),From);
         if (Found) {
            return Found;
         }
         if (To && To == From) {
            break;
         }
         From = this->next(From);
      }
      return NULL;
   }

protected:
   virtual void destroyItem(COLhashmapPlace Place) {
      delete (COLhashmapNode<KeyType, Type>*) Place;
   }
   virtual COLindex hashItem(COLhashmapPlace Place) const {
      return ((COLhashmapNode<KeyType, Type>*) Place)->Hash;
   }
   virtual bool equalKey(const void* Key, COLhashmapPlace Place) const {
      return CompareType::equalKey(*(KeyType*) Key,((COLhashmapNode<KeyType, Type>*) Place)->Key);
   }
private:
   HashProc Hash;
};

template <class K, class V, class Cmp>
COLhashmap<K,V,Cmp>& COLhashmap<K,V,Cmp>::operator=(const COLhashmap<K,V,Cmp>& That) {
   COLhashmapPlace Place;
   this->clear();
   for(Place = That.first(); Place; Place = That.next(Place)) {
      this->add(That.key(Place), That[Place]);
   }
   return *this;
}

template <class KeyType, class Type, class CompareType>
Type& COLhashmap<KeyType, Type, CompareType>::operator[](const KeyType& Key) {
   const COLindex HashValue = Hash(Key);
   COLhashmapPlace Place = this->findItem(HashValue,&Key);
   if (Place == NULL) {
      Place = this->addItem(HashValue, &Key, new COLhashmapNode<KeyType, Type>(HashValue, Key, Type()));
   }
   return ((COLhashmapNode<KeyType, Type>*) Place)->Item;
}

template <class KeyType, class Type, class CompareType>
const Type& COLhashmap<KeyType, Type, CompareType>::operator[](const KeyType& Key) const {
   COLhashmapPlace Place = this->find(Key);
   COLPRECONDITION(Place != NULL);
   return ((COLhashmapNode<KeyType, Type>*) Place)->Item;
}

template <class KeyType, class Type, class CompareType>
COLhashmapPlace COLhashmap<KeyType, Type, CompareType>::add(const KeyType& Key, const Type& Item) {
   COLindex HashValue = Hash(Key);
   return this->addItem(HashValue, &Key, new COLhashmapNode<KeyType, Type>(HashValue, Key, Item));
}

#endif // end of defensive include

