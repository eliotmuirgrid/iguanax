#ifndef __COL_LIST_H__
#define __COL_LIST_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:      COLlist
//
// Description:
//
// Eliot - very much worth having a read through this class.
//
// Templated double linked list class patterned after STL list.
//
// See http://www.cplusplus.com/reference/list/list/ for examples.
//
// The objects stored in the template must support the copy contructor
// Type(const Type&)
//
// A copy is made of an item when it is inserted into the list.
// When an item is removed from the list it is destructed.
//
// Iteration with COLlistPlace is deprecated for new code.
// Use iterator or const_iterator instead.
//
// Author: Ian Farquharson
// Date:   02/05/2004
//
// Author: Andrew Vajoczki - added STL style methods and iterator
//---------------------------------------------------------------------------
#include <COL/COLerror.h>

struct COLlistNode {
   COLlistNode*  Previous;
   COLlistNode*  Next;
};

typedef COLlistNode* COLlistPlace;

class COLvoidList{
private:
   COLindex      Size;
   COLlistPlace  Head;
   COLlistPlace  Tail;
public:
   COLvoidList();
   virtual ~COLvoidList() {}
   COLindex size() const {return Size;}
   void clear();
   void remove(COLlistPlace Place);
   void exchange(COLlistPlace Place1, COLlistPlace Place2);
   COLlistPlace first() const { return Head; }
   COLlistPlace last() const { return Tail; }
   COLlistPlace previous(COLlistPlace Place) const;
   COLlistPlace next(COLlistPlace Place) const;
protected:
   COLlistPlace insertItem(COLlistPlace Item, COLlistPlace Place);
   COLlistPlace addItem(COLlistPlace Item);
protected:
   virtual void destroyItem(COLlistPlace Place) = 0;
private:
#ifdef COL_ENABLE_INVARIANT_INTEGRITY_CHECK
   void invariant() const;
#else
   void invariant() const {}
#endif
   COLvoidList(const COLvoidList& list);               // disable copy constructor
   COLvoidList& operator=(const COLvoidList& list);   // disable assignment
};

template <class Type>
struct COLlistItemNode: public COLlistNode{
   Type Item;
   COLlistItemNode(const Type& TheItem): Item(TheItem) {}
   COLlistItemNode() : Item() {}
};

template <class Type>
class COLlist : public COLvoidList{
public:
   struct iterator {
      COLlist<Type>* list_;
      COLlistPlace place_;
      iterator()
      : list_(NULL)
      , place_(NULL) {
      }
      iterator(const iterator& that)
      : list_(that.list_)
      , place_(that.place_) {
      }
      iterator& operator=(const iterator& that) {
         list_ = that.list_;
         place_ = that.place_;
         return *this;
      }
      iterator& operator++() {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         place_ = list_->next(place_);
         return *this;
      }
      iterator operator++(int) {
         iterator tmp(*this);
         ++*this;
         return tmp;
      }
      iterator& operator--() {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         place_ = list_->previous(place_);
         return *this;
      }
      iterator operator--(int) {
         iterator tmp(*this);
         --*this;
         return tmp;
      }
      Type& operator*() {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         return list_->operator[](place_);
      }
      Type* operator->() {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         return &(list_->operator[](place_));
      }
      bool operator==(const iterator& that) const {
         return that.list_  == this->list_
             && that.place_ == this->place_;
      }
      bool operator!=(const iterator& that) const {
         return !operator==(that);
      }
      // only to be used by COLlist class itself
      iterator(COLlist<Type>* list, COLlistPlace place) : list_(list) , place_(place) { }
   };

   struct const_iterator {
      const COLlist<Type>* list_;
      COLlistPlace place_;
      const_iterator()
      : list_(NULL)
      , place_(NULL) {
      }
      const_iterator(const const_iterator& that)
      : list_(that.list_)
      , place_(that.place_) {
      }
      const_iterator(const iterator& that)
      : list_(that.list_)
      , place_(that.place_) {
      }
      const_iterator& operator=(const const_iterator& that) {
         list_ = that.list_;
         place_ = that.place_;
         return *this;
      }
      const_iterator& operator=(const iterator& that) {
         list_ = that.list_;
         place_ = that.place_;
         return *this;
      }
      const_iterator& operator++() {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         place_ = list_->next(place_);
         return *this;
      }
      const_iterator operator++(int) {
         const_iterator tmp(*this);
         ++*this;
         return tmp;
      }
      const_iterator& operator--() {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         place_ = list_->previous(place_);
         return *this;
      }
      const_iterator operator--(int) {
         const_iterator tmp(*this);
         --*this;
         return tmp;
      }
      const Type& operator*() const {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         return list_->operator[](place_);
      }
      const Type* operator->() const {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         return &(list_->operator[](place_));
      }
      bool operator==(const const_iterator& that) const {
         return that.list_  == this->list_
             && that.place_ == this->place_;
      }
      bool operator!=(const const_iterator& that) const {
         return !operator==(that);
      }
      // only to be used by COLlist class itself
      const_iterator(const COLlist<Type>* list, COLlistPlace place) : list_(list) , place_(place) { }
   };

   struct reverse_iterator {
      COLlist<Type>* list_;
      COLlistPlace place_;
      reverse_iterator()
      : list_(NULL)
      , place_(NULL) {
      }
      reverse_iterator(const reverse_iterator& that)
      : list_(that.list_)
      , place_(that.place_) {
      }
      reverse_iterator& operator=(const reverse_iterator& that) {
         list_ = that.list_;
         place_ = that.place_;
         return *this;
      }
      reverse_iterator& operator++() {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         place_ = list_->previous(place_);
         return *this;
      }
      reverse_iterator operator++(int) {
         reverse_iterator tmp(*this);
         ++*this;
         return tmp;
      }
      reverse_iterator& operator--() {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         place_ = list_->next(place_);
         return *this;
      }
      reverse_iterator operator--(int) {
         reverse_iterator tmp(*this);
         --*this;
         return tmp;
      }
      Type& operator*() {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         return list_->operator[](place_);
      }
      Type* operator->() {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         return &(list_->operator[](place_));
      }
      bool operator==(const reverse_iterator& that) const {
         return that.list_  == this->list_
             && that.place_ == this->place_;
      }
      bool operator!=(const reverse_iterator& that) const {
         return !operator==(that);
      }
      // only to be used by COLlist class itself
      reverse_iterator(COLlist<Type>* list, COLlistPlace place) : list_(list) , place_(place) { }
   };

   struct const_reverse_iterator {
      const COLlist<Type>* list_;
      COLlistPlace place_;
      const_reverse_iterator()
      : list_(NULL)
      , place_(NULL) {
      }
      const_reverse_iterator(const const_reverse_iterator& that)
      : list_(that.list_)
      , place_(that.place_) {
      }
      const_reverse_iterator(const reverse_iterator& that)
      : list_(that.list_)
      , place_(that.place_) {
      }
      const_reverse_iterator& operator=(const const_reverse_iterator& that) {
         list_ = that.list_;
         place_ = that.place_;
         return *this;
      }
      const_reverse_iterator& operator=(const reverse_iterator& that) {
         list_ = that.list_;
         place_ = that.place_;
         return *this;
      }
      const_reverse_iterator& operator++() {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         place_ = list_->previous(place_);
         return *this;
      }
      const_reverse_iterator operator++(int) {
         const_reverse_iterator tmp(*this);
         ++*this;
         return tmp;
      }
      const_reverse_iterator& operator--() {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         place_ = list_->next(place_);
         return *this;
      }
      const_reverse_iterator operator--(int) {
         const_reverse_iterator tmp(*this);
         --*this;
         return tmp;
      }
      const Type& operator*() {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         return list_->operator[](place_);
      }
      const Type* operator->() {
         COLASSERT(list_);
         COLPRECONDITION(place_);
         return &(list_->operator[](place_));
      }
      bool operator==(const const_reverse_iterator& that) const {
         return that.list_  == this->list_
             && that.place_ == this->place_;
      }
      bool operator!=(const const_reverse_iterator& that) const {
         return !operator==(that);
      }
      // only to be used by COLlist class itself
      const_reverse_iterator(const COLlist<Type>* list, COLlistPlace place) : list_(list) , place_(place) { }
   };

public:
   //public typedefs for generic functions
   typedef Type                                     TValue;
   typedef COLlistPlace                             TPlace;

   typedef Type        value_type;
   typedef Type&       reference;
   typedef const Type& const_reference;
   typedef Type*       pointer;
   typedef const Type* const_pointer;

   virtual ~COLlist() {
      try {
         this->clear(); 
      }
      catch (const COLerror& Error) {
         //COL_LOG(Error);
      }
   }
   Type& operator[](COLlistPlace Place) {
      COLPRECONDITION(Place != NULL);
      return ((COLlistItemNode<Type>*) Place)->Item;
   }
   const Type& operator[](COLlistPlace Place) const {
      COLPRECONDITION(Place != NULL);
      return ((COLlistItemNode<Type>*) Place)->Item;
   }
   COLlistPlace insert(const Type& Item, COLlistPlace Place) {
      return this->insertItem(new COLlistItemNode<Type>(Item), Place);
   }
   COLlistPlace add(const Type& Item) {
      return this->addItem(new COLlistItemNode<Type>(Item));
   }
   COLlistPlace add() {
      return this->addItem(new COLlistItemNode<Type>());
   }

   bool empty() const { return 0 == this->size(); }

   const Type& front() const { return operator[](this->first()); }
   Type&       front()       { return operator[](this->first()); }

   const Type& back() const { return operator[](this->last()); }
   Type&       back()       { return operator[](this->last()); }

   const_iterator begin() const { return const_iterator(this, this->first()); }
   iterator       begin()       { return iterator(this, this->first()); }

   const_iterator end() const { return const_iterator(this, NULL); }
   iterator       end()       { return iterator(this, NULL); }

   const_reverse_iterator rbegin() const { return const_reverse_iterator(this, this->last()); }
   reverse_iterator       rbegin()       { return reverse_iterator(this, this->last()); }

   const_reverse_iterator rend() const { return const_reverse_iterator(this, NULL); }
   reverse_iterator       rend()       { return reverse_iterator(this, NULL); }

   const_iterator cbegin() const { return const_iterator(this, this->first()); }
   const_iterator cend()   const { return const_iterator(this, NULL); }

   const_reverse_iterator crbegin() const { return const_reverse_iterator(this, this->last()); }
   const_reverse_iterator crend()   const { return const_reverse_iterator(this, NULL); }

   iterator insert(iterator position, const Type& value) {
      COLASSERT(this == position.list_);
      COLlistPlace NewPlace = this->insert(value, position.place_);
      return iterator(this, NewPlace);
   }

   void push_front(const Type& value) { this->insert(value, this->first()); }

   void push_back(const Type& value) { this->add(value); }

   Type& push_back() {
      this->add(Type());
      return this->back();
   }

   iterator erase(iterator position) {
      COLASSERT(this == position.list_);
      COLPRECONDITION(position.place_);
      COLlistPlace Next = this->next(position.place_);
      this->remove(position.place_);
      return iterator(this, Next);
   }

   void pop_front() { this->erase(begin()); }
   void pop_back()  { this->erase(iterator(this,this->last())); }

protected:
   virtual void destroyItem(COLlistPlace Place) { delete (COLlistItemNode<Type>*) Place; }
};

#endif // end of defensive include