//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLlist
//
// Description:
//
// Templated double linked list class
//
// Author: Ian Farquharson
// Date:   02/05/2004
//---------------------------------------------------------------------------
#include "COLerror.h"
#include "COLlist.h"

COLvoidList::COLvoidList(): Size(0), Head(NULL), Tail(NULL){}


void COLvoidList::clear(){
   COLlistPlace Place = Head;
   while (Place != NULL){
      COLlistPlace Current = Place;
      Place = Place->Next;
      destroyItem(Current);
   }
   Size = 0;
   Head = NULL;
   Tail = NULL;
   invariant();
}

void COLvoidList::remove(COLlistPlace Place){
   COLPRECONDITION(Place != NULL);
   if (Place == NULL){
      return; // TODO not needed if precondition is used
   }

   if (Place->Previous == NULL){
      Head = Place->Next;
   } else {
      Place->Previous->Next = Place->Next;
   }

   if (Place->Next == NULL) {
      Tail = Place->Previous;
   } else{
      Place->Next->Previous = Place->Previous;
   }

   destroyItem(Place);
   Size--;
   invariant();
}

void COLvoidList::exchange(COLlistPlace Place1, COLlistPlace Place2){
   COLPRECONDITION(Place1 != NULL && Place2 != NULL);
   if (Place1 == NULL || Place2 == NULL){
      return; // TODO not needed if precondition is used
   }

   if (Place1 != Place2){
      COLlistPlace Previous = Place1->Previous;
      COLlistPlace Next = Place1->Next;
      Place1->Previous = Place2->Previous;
      Place1->Next = Place2->Next;
      Place2->Previous = Previous;
      Place2->Next = Next;

      if (Place1->Previous != NULL){
         if (Place1->Previous == Place1){
            Place1->Previous = Place2;
         } else {
            Place1->Previous->Next = Place1;
         }
      }

      if (Place1->Next != NULL){
         if (Place1->Next == Place1){
            Place1->Next = Place2;
         } else {
            Place1->Next->Previous = Place1;
         }
      }

      if (Place2->Previous != NULL) {
         if (Place2->Previous == Place2) {
            Place2->Previous = Place1;
         } else {
            Place2->Previous->Next = Place2;
         }
      }

      if (Place2->Next != NULL) {
         if (Place2->Next == Place2){
            Place2->Next = Place1;
         } else {
            Place2->Next->Previous = Place2;
         }
      }

      if (Head == Place1){
         Head = Place2;
      } else if (Head == Place2){
         Head = Place1;
      }

      if (Tail == Place1){
         Tail = Place2;
      } else if (Tail == Place2) {
         Tail = Place1;
      }
      invariant();
   }
}

COLlistPlace COLvoidList::previous(COLlistPlace Place) const{
   return Place == NULL ? Tail : Place->Previous;
}

COLlistPlace COLvoidList::next(COLlistPlace Place) const{
   return Place == NULL ? Head : Place->Next;
}

COLlistPlace COLvoidList::insertItem(COLlistPlace Item, COLlistPlace Place){
   COLPRECONDITION(Item != NULL);
   if (Item == NULL) {
      COL_ERROR_STREAM("Out of memory", COLerror::SystemError);
   }
   if (Place == NULL){
      return addItem(Item);
   }
   Item->Previous = Place->Previous;
   Item->Next = Place;
   if (Item->Previous == NULL){
      Head = Item;
   } else {
      Item->Previous->Next = Item;
   }
   Place->Previous = Item;
   Size++;
   invariant();
   return Item;
}

COLlistPlace COLvoidList::addItem(COLlistPlace Item){
   COLPRECONDITION(Item != NULL);
   if (Item == NULL) {
      COL_ERROR_STREAM("Out of memory", COLerror::SystemError);
   }
   Item->Previous = Tail;
   Item->Next = NULL;
   if (Tail == NULL){
      Head = Item;
   } else {
      Tail->Next = Item;
   }
   Tail = Item;
   Size++;
   invariant();
   return Item;
}

#ifdef COL_ENABLE_INVARIANT_INTEGRITY_CHECK
void COLvoidList::invariant() const{
   if (Size == 0) {
      COLASSERT(Head == NULL && Tail == NULL);
   } else {
      COLASSERT(Head != NULL && Tail != NULL);
      COLASSERT(Head->Previous == NULL && Tail->Next == NULL);
   }
   COLlistPlace Place = Head;
   for (COLindex i = 0; i < Size; i++){
      COLASSERT(Place != NULL);
      if (Place->Next == NULL) {
         COLASSERT(Place == Tail);
      } else {
         COLASSERT(Place->Next->Previous == Place);
      }
      Place = Place->Next;
   }
   COLASSERT(Place == NULL);
}
#endif // DEBUG