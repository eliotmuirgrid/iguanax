//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLmap
//
// Description:
//
// Templated sorted tree-based map container
//
// Author: Ian Farquharson
// Date:   02/05/2004
//---------------------------------------------------------------------------
#include "COLerror.h"
#include "COLmap.h"

/***************************\
*                           *
* AVL Tree collection class *
*                           *
\***************************/

// COLavlTree is Copyright (c) 1994, 1995  L.K.Prentice. All rights reserved.

// Rotation Primitives

static void Balance1(COLavlTreePlace& p, bool& h)
{
   COLavlTreePlace p1;
   COLavlTreePlace p2;
   int      b1;     // -1..+1
   int      b2;     // -1..+1

   //  h == true ==> Left branch has become less high
   switch (p->Balance)
   {
   case (-1):
      p->Balance = 0;
      break;

   case (0) :
      p->Balance = 1;
      h = false;
      break;

   case (1) :
      // rebalance
      p1 = p->Right;
      b1 = p1->Balance;
      if (b1 >= 0)
      {
         /********************\
         *                    *
         * single RR rotation *
         *                    *
         *             pR     *
         *           /    \   *
         *         p          *
         *       /   \        *
         *           pRL      *
         *                    *
         \********************/
         p->Right = p1->Left;
         p1->Left = p;
         if (b1 == 0)
         {
            p->Balance = 1;
            p1->Balance = (-1);
            h = false;
         }
         else
         {
            p->Balance = 0;
            p1->Balance = 0;
         }
         p = p1;
         // now do the ups
         p->Up = p->Left->Up;
         p->Left->Up = p;
         if (p->Left->Right != NULL)
            p->Left->Right->Up = p->Left;
      }
      else
      {
         /************************\
         *                        *
         * Double RL Rotation     *
         *             pRL        *
         *          /       \     *
         *        p          pR   *
         *      /   \      /    \ *
         *         pRLL  p        *
         *                        *
         \************************/
         p2 = p1->Left;
         b2 = p2->Balance;
         p1->Left = p2->Right;
         p2->Right = p1;
         p->Right = p2->Left;
         p2->Left = p;
         if (b2 == 1)
            p->Balance = (-1);
         else
            p->Balance = 0;
         if (b2 == (-1))
            p1->Balance = 1;
         else
            p1->Balance = 0;
         p = p2;
         p2->Balance = 0;
         // now do the ups
         p->Up = p->Left->Up;
         p->Left->Up = p;
         if (p->Left->Right)
            p->Left->Right->Up = p->Left;
         p->Right->Up = p;
         if (p->Right->Left)
            p->Right->Left->Up = p->Right;
      }
   }
}

static void Balance2(COLavlTreePlace& p, bool& h)
{
   COLavlTreePlace p1;
   COLavlTreePlace p2;
   int      b1;     // -1..+1
   int      b2;     // -1..+1

   //  h == true ==> Right branch has become less high
   switch (p->Balance)
   {
   case 1 :
      p->Balance = 0;
      break;

   case 0 :
      p->Balance =(-1);
      h = false;
      break;

   case -1:
      p1 = p->Left;
      b1 = p1->Balance;
      if (b1 <= 0)
      {
         /*********************\
         *                     *
         *  single LL rotation *
         *             pL      *
         *           /    \    *
         *                 p   *
         *                /    *
         *              pLR    *
         *                     *
         \*********************/
         p->Left = p1->Right;
         p1->Right = p;
         if (b1 == 0)
         {
            p->Balance = (-1);
            p1->Balance = 1;
            h = false;
         }
         else
         {
            p->Balance = 0;
            p1->Balance = 0;
         }
         p = p1;
         // now do the ups
         p->Up = p->Right->Up;
         p->Right->Up = p;
         if (p->Right->Left)
            p->Right->Left->Up = p->Right;
      }
      else
      {
         /************************\
         *                        *
         *  Double LR Rotation    *
         *             pLR        *
         *          /       \     *
         *        pL          p   *
         *      /   \      /    \ *
         *         pLRL  pLRR     *
         *                        *
         \************************/
         p2 = p1->Right;
         b2 = p2->Balance;
         p1->Right= p2->Left;
         p2->Left = p1;
         p->Left = p2->Right;
         p2->Right = p;
         if (b2 == (-1))
            p->Balance = 1;
         else
            p->Balance = 0;
         if (b2 == 1)
            p1->Balance = (-1);
         else
            p1->Balance = 0;
         p = p2;
         p2->Balance = 0;
         // now do the ups
         p->Up = p->Right->Up;
         p->Left->Up = p;
         if (p->Left->Right)
            p->Left->Right->Up = p->Left;
         p->Right->Up = p;
         if (p->Right->Left)
            p->Right->Left->Up = p->Right;
      }
   }
}

static bool Del(COLavlTreePlace& r, COLavlTreePlace& q, bool& h)
{
   if (r->Right)
   {
      // now going Right until we can't
      bool res = Del(r->Right, q, h);
      if (q->Left == r)
      {
         if (h)
            Balance2(q->Left, h);
      }
      else if (h)
         Balance2(r, h);
      return res;
   }
   else
   {
      // copy the data from node r to q 'position'
      COLavlTreePlace qq    = q;
      COLavlTreePlace rr    = r;
      COLavlTreePlace qup   = q->Up;
      COLavlTreePlace qprev = q->Left;
      COLavlTreePlace qnext = q->Right;
      int          qBal  = q->Balance;
      COLavlTreePlace rup   = r->Up;
      COLavlTreePlace rprev = r->Left;
      q = rr;
      q->Balance = qBal;
      q->Up   = qup;
      if (q->Up){
         if (q->Up->Left == qq){
            q->Up->Left = q;
         } else {
            q->Up->Right = q;
         }
      }
      if (qnext != rr)
      {
         q->Right = qnext;
         if (q->Right)
            q->Right->Up = q;
      }
      if (qprev != rr)
      {
         q->Left = qprev;
         if (q->Left)
            q->Left->Up = q;
      }
      if (rup->Right == rr){
         if (rprev)
         {
            rup->Right = rprev;
            rprev->Up = rup;
         }
         else{
            rup->Right = NULL;
         }  
      }
      r = rprev;

      h = true;
      return true;
   }
}

// COLavlTreeNode

COLavlTreeNode::COLavlTreeNode(void):
   Up(NULL),
   Left(NULL),
   Right(NULL),
   Balance(0)
{
}

// COLavlTreeBase

COLavlTreeBase::COLavlTreeBase(void):
   RootNode(NULL),
   NodeCount(0)
{
}

void COLavlTreeBase::clear(void)
{
   if (RootNode != NULL)
   {
      COLavlTreePlace curr = RootNode;
      COLavlTreePlace parent;
      while (true)
      {
         while (true)  // find a leaf
         {
            if (curr->Left != NULL)
            {
               curr = curr->Left;
               continue;
            }
            if (curr->Right != NULL)
            {
               curr = curr->Right;
               continue;
            }
            break;    // now at a leaf since both Left and Right pointers are NULL
         }

         if ((parent = curr->Up) == NULL)
         {
            destroyItem(curr);  // only the root of the tree is Left, delete it and break out of loop
            break;
         }
         else
         {
            if (parent->Left == curr) // Set the appropriate pointer to NULL, The parent may become a leaf
               parent->Left = NULL;
            else
               parent->Right = NULL;
            destroyItem(curr);
            curr = parent;
         }
      }
   }
   RootNode = NULL;
   NodeCount = 0;
}

struct COLavlTreeBaseContext           // used for zapping a node
{
   COLavlTreePlace   stack[32];    // path to the target node
   int            stack_count;  // number of items in path
   int            stack_place;  // where we are Up to
   COLavlTreePlace   target_node;  // target node
};

void COLavlTreeBase::remove(COLavlTreePlace Place)
{
   COLPRECONDITION(Place != NULL);
   bool Higher = false;
   COLavlTreeBaseContext Context;
   memset(&Context, 0, sizeof(Context));
   for (COLavlTreePlace p = Place; p != RootNode; p = p->Up)
   {
      Context.stack[Context.stack_count] = p;
      Context.stack_count++;
   }
   Context.stack_place = Context.stack_count;
   Context.target_node = Place;
   if (zapIt(Context, RootNode, Higher))
      NodeCount--;
   invariant();
}

COLavlTreePlace COLavlTreeBase::first(void) const
{
   return downLeft(RootNode);
}

COLavlTreePlace COLavlTreeBase::last(void) const
{
   return downRight(RootNode);
}

COLavlTreePlace COLavlTreeBase::previous(COLavlTreePlace Place) const
{
   if (Place == NULL)
      return last();
   if (Place->Left != NULL)
      return downRight(Place->Left);
   return upLeft(Place);
}

COLavlTreePlace COLavlTreeBase::next(COLavlTreePlace Place) const
{
   if (Place == NULL)
      return first();
   if (Place->Right != NULL)
      return downLeft(Place->Right);
   return upRight(Place);
}

bool COLavlTreeBase::removeItem(const void* Key)
{
   bool Higher = false;
   if (zapIt(Key, RootNode, Higher))
   {
      NodeCount--;
      invariant();
      return true;
   }
   return false;
}

COLavlTreePlace COLavlTreeBase::addItem(const void* Key, COLavlTreePlace Item)
{
   bool new_level = false;
   COLavlTreePlace Place = insertIt(Key, Item, RootNode, NULL, new_level, false);
   if (Place == NULL)
   {
      destroyItem(Item);
      return NULL;
   }
   NodeCount++;
   invariant();
   return Place;
}

COLavlTreePlace COLavlTreeBase::addUniqueItem(const void* Key, COLavlTreePlace Item)
{
   bool new_level = false;
   COLavlTreePlace Place = insertIt(Key, Item, RootNode, NULL, new_level, true);
   if (Place == NULL)
   {
      destroyItem(Item);
      return NULL;
   }
   NodeCount++;
   invariant();
   return Place;
}

COLavlTreePlace COLavlTreeBase::findFirstItem(const void* Key) const
{
   COLavlTreePlace Place = findBelowItem(Key);
   if (Place) Place = next(Place);
   else Place = first();
   if (Place && compareKey(Key,Place) == 0) return Place;
   return 0;
}

COLavlTreePlace COLavlTreeBase::findItem(const void* Key) const
{
   COLavlTreePlace curr = RootNode;
   while (curr != NULL)
   {
      int comparism = compareKey(Key, curr);
      if (comparism < 0)
         curr = curr->Left;
      else if (comparism > 0)
         curr = curr->Right;
      else
         break; // found it
   }
   return curr;
}

COLavlTreePlace COLavlTreeBase::findNearestItem(const void* Key) const
{
   COLavlTreePlace curr = RootNode;
   while (curr != NULL)
   {
      int comparism = compareKey(Key, curr);
      if (comparism < 0)
         if (curr->Left != NULL)
            curr = curr->Left;
         else
            break; // found nearest
      else if (comparism > 0)
         if (curr->Right != NULL)
            curr = curr->Right;
         else
         {
            COLavlTreePlace p = next(curr);
            if (p != NULL)
               curr = p;
            break; // found nearest
         }
      else
         break; // found it
   }
   return curr;
}

COLavlTreePlace COLavlTreeBase::findBelowItem(const void* Key) const
{
   COLavlTreePlace curr = RootNode;
   while (curr != NULL)
   {
      int comparism = compareKey(Key, curr);
      if (comparism <= 0)
         if (curr->Left != NULL)
            curr = curr->Left;
         else
         {
            curr = previous(curr);
            break; // found below
         }
      else
         if (curr->Right != NULL)
         curr = curr->Right;
      else
         break; // found below
   }
   return curr;
}

COLavlTreePlace COLavlTreeBase::findAboveItem(const void* Key) const
{
   COLavlTreePlace curr = RootNode;
   while (curr != NULL)
   {
      int comparism = compareKey(Key, curr);
      if (comparism >= 0)
         if (curr->Right != NULL)
            curr = curr->Right;
         else
         {
            curr = next(curr);
            break; // found above
         }
      else
         if (curr->Left != NULL)
         curr = curr->Left;
      else
         break; // found above
   }
   return curr;
}

COLavlTreePlace COLavlTreeBase::downLeft(COLavlTreePlace Place) const
{
   if (Place != NULL)
      while (Place->Left != NULL)
         Place = Place->Left;
   return Place;
}

COLavlTreePlace COLavlTreeBase::downRight(COLavlTreePlace Place) const
{
   if (Place != NULL)
      while (Place->Right != NULL)
         Place = Place->Right;
   return Place;
}

COLavlTreePlace COLavlTreeBase::upLeft(COLavlTreePlace Place) const
{
   if (Place != NULL)
      while (Place->Up != NULL && Place->Up->Left == Place)
         Place = Place->Up;
   Place = Place->Up;
   return Place;
}

COLavlTreePlace COLavlTreeBase::upRight(COLavlTreePlace Place) const
{
   if (Place != NULL)
      while (Place->Up != NULL && Place->Up->Right == Place)
         Place = Place->Up;
   Place = Place->Up;
   return Place;
}

COLavlTreePlace COLavlTreeBase::insertIt(const void* Key,
                                   COLavlTreePlace Item,
                                   COLavlTreePlace& CurrNode,    // tree top/ where found
                                   COLavlTreePlace NodeAbove,    // for Up
                                   bool& Higher,   // true if new level added
                                   bool Unique)
{
   COLavlTreePlace p1;
   COLavlTreePlace p2;
   int comparism;
   COLavlTreePlace result;

   Higher = false;

   if (CurrNode == NULL)        // check if we are empty - in which case insert
   {
      CurrNode = Item;
      CurrNode->Left = NULL;
      CurrNode->Right = NULL;
      CurrNode->Up = NodeAbove;   // from recursion
      CurrNode->Balance = 0;       // no need to Balance this node
      Higher = true;
      return CurrNode;           // back Up recursion
   }

   // otherwise we still want to look down

   comparism = compareKey(Key, CurrNode);
   if (comparism < 0)
   {
      result = insertIt(Key, Item, CurrNode->Left, CurrNode, Higher, Unique); // recurse
      if (Higher)
      {
         if (CurrNode->Balance == 1)
         {
            CurrNode->Balance = 0; Higher = false;
         }
         else if (CurrNode->Balance == 0)
            CurrNode->Balance = -1;   // pass Up
         else  // rebalance this section of the tree
         {
            p1 = CurrNode->Left;
            if (p1->Balance == -1)
            {
               // single LL rotation
               CurrNode->Left = p1->Right;
               p1->Right = CurrNode;
               CurrNode->Balance = 0;
               CurrNode = p1;
               CurrNode->Up = CurrNode->Right->Up;  // put back the ups
               CurrNode->Right->Up = CurrNode;
               if (CurrNode->Right->Left)
                  CurrNode->Right->Left->Up = CurrNode->Right;
            }
            else
            {
               // double LR rotation
               p2 = p1->Right;
               p1->Right = p2->Left;
               p2->Left = p1;
               CurrNode->Left = p2->Right;
               p2->Right = CurrNode;
               if (p2->Balance == -1)
                  CurrNode->Balance = 1;
               else
                  CurrNode->Balance = 0;
               if (p2->Balance == 1)
                  p1->Balance = -1;
               else
                  p1->Balance = 0;
               CurrNode = p2;
               CurrNode->Up = CurrNode->Right->Up;     // now return ups
               CurrNode->Right->Up = CurrNode;
               if (CurrNode->Right->Left)
                  CurrNode->Right->Left->Up = CurrNode->Right;
               CurrNode->Left->Up = CurrNode;
               if (CurrNode->Left->Right)
                  CurrNode->Left->Right->Up = CurrNode->Left;
            }
            CurrNode->Balance = 0;
            Higher = false;
         }
      } // Higher
   }
   else if (comparism > 0 || !Unique)
   {
      result = insertIt(Key, Item, CurrNode->Right, CurrNode, Higher, Unique);
      if (Higher)
      {                 // put in Higher - prev Branch High
         if (CurrNode->Balance < 0)
         {
            CurrNode->Balance = 0;
            Higher = false;
         }
         else if (CurrNode->Balance == 0)
            CurrNode->Balance = 1;   // rebalance further Up tree
         else
         {
            p1 = CurrNode->Right;             // rebalance now
            if (p1->Balance == 1)            // single RR rotation
            {
               CurrNode->Right = p1->Left;
               p1->Left = CurrNode;
               CurrNode->Balance = 0;
               CurrNode = p1;
               CurrNode->Up = CurrNode->Left->Up;       // restore the ups
               CurrNode->Left->Up = CurrNode;
               if (CurrNode->Left->Right)         // startup cond
                  CurrNode->Left->Right->Up = CurrNode->Left;
            }
            else
            {                         // double RL Rotation
               p2 = p1->Left;
               p1->Left = p2->Right;
               p2->Right = p1;
               CurrNode->Right = p2->Left;
               p2->Left = CurrNode;
               if (p2->Balance == +1)
                  CurrNode->Balance = -1;
               else
                  CurrNode->Balance = 0;
               if (p2->Balance == -1)
                  p1->Balance = 1;
               else
                  p1->Balance = 0;
               CurrNode = p2;
               CurrNode->Up = CurrNode->Left->Up;
               CurrNode->Left->Up = CurrNode;
               if (CurrNode->Left->Right != NULL)
                  CurrNode->Left->Right->Up = CurrNode->Left;
               CurrNode->Right->Up = CurrNode;
               if (CurrNode->Right->Left != NULL)
                  CurrNode->Right->Left->Up = CurrNode->Right;
            }
            CurrNode->Balance = 0;
            Higher = false;
         }
      } // Higher
   }
   else // comparism == 0
   {
      return NULL;
   }
   return result;
}

bool COLavlTreeBase::zapIt(COLavlTreeBaseContext& Context, // path down tree to target node
                        COLavlTreePlace& CurrNode,  // tree top/ where found
                        bool& Higher)  // true if new level added
{
   COLavlTreePlace kill_node;
   COLavlTreePlace p = NULL;
   bool result = false;

   if (CurrNode == NULL)
   {
      Higher = false;
      return result;
   }

   Context.stack_place--;
   if (Context.stack_place < 0)
   {
      COLPOSTCONDITION(CurrNode == Context.target_node);
      p = CurrNode;  // THIS CODE IS REPEATED IN BOTH kILLiT FUNCTIONS
      if (!p->Right)  // nothing on Right - just attach properly
      {
         if (p->Left)
            p->Left->Up = CurrNode->Up;
         CurrNode = p->Left;
         Higher = true;
         result = true;
         kill_node = p;
      }
      else if (!p->Left)  // alternatively nothing on prev - just attach
      {
         if (p->Right)
            p->Right->Up = CurrNode->Up;
         CurrNode = p->Right;
         Higher = true;
         result = true;
         kill_node = p;
      }
      else  // nothing either side - ok this is a pain - recurse !!
      {
         kill_node = p;
         result = Del(p->Left, p, Higher); // copy of killed node. Can result ever be false?!?
         CurrNode = p;
         if (Higher)
            Balance1(CurrNode, Higher);
      }
      destroyItem(kill_node);
   }
   else if (CurrNode->Left == Context.stack[Context.stack_place])
   {
      result = zapIt(Context, CurrNode->Left, Higher);
      if (Higher)
         Balance1(CurrNode, Higher);
   }
   else if (CurrNode->Right == Context.stack[Context.stack_place])
   {
      result = zapIt(Context, CurrNode->Right, Higher);
      if (Higher)
         Balance2(CurrNode, Higher);
   }
   else
   {
      ; // DiagFail();
   }
   return result;
}

bool COLavlTreeBase::zapIt(const void* Key,  // search for a Key
                        COLavlTreePlace& CurrNode,  // tree top/ where found
                        bool& Higher)  // true if new level added
{
   COLavlTreePlace kill_node;
   COLavlTreePlace p = NULL;
   int comparism;
   bool result = false;

   if (CurrNode == NULL)
   {
      Higher = false;
      return result;
   }

   comparism = compareKey(Key, CurrNode);
   if (comparism < 0)
   {
      result = zapIt(Key, CurrNode->Left, Higher);
      if (Higher)
         Balance1(CurrNode, Higher);
   }
   else if (comparism > 0)
   {
      result = zapIt(Key, CurrNode->Right, Higher);
      if (Higher)
         Balance2(CurrNode, Higher);
   }
   else
   {
      p = CurrNode;  // THIS CODE IS REPEATED IN BOTH kILLiT FUNCTIONS
      if (!p->Right)  // nothing on Right - just attach properly
      {
         if (p->Left)
            p->Left->Up = CurrNode->Up;
         CurrNode = p->Left;
         Higher = true;
         result = true;
         kill_node = p;
      }
      else if (!p->Left)  // alternatively nothing on prev - just attach
      {
         if (p->Right)
            p->Right->Up = CurrNode->Up;
         CurrNode = p->Right;
         Higher = true;
         result = true;
         kill_node = p;
      }
      else  // nothing either side - ok this is a pain - recurse !!
      {
         kill_node = p;
         result = Del(p->Left, p, Higher); // copy of killed node. Can result ever be false?!?
         CurrNode = p;
         if (Higher)
            Balance1(CurrNode, Higher);
      }
      destroyItem(kill_node);
   }
   return result;
}

COLostream& operator<<(COLostream& Stream, const COLmap<COLstring, COLstring>& Map) {
   Stream << "MAP size=" << Map.size() << newline;
   for(auto i = Map.cbegin(); i != Map.cend(); i++) { 
      Stream << "\t" << i->first << "=" << i->second << newline; 
   }
   return Stream;
}

#ifdef COL_ENABLE_INVARIANT_INTEGRITY_CHECK

void COLavlTreeBase::testLinkage(COLavlTreePlace Place) const
{
   if (Place == NULL)
      return;
   COLASSERT(Place->Left == NULL || Place->Left->Up == Place);
   COLASSERT(Place->Right == NULL || Place->Right->Up == Place);
   testLinkage(Place->Left);
   testLinkage(Place->Right);
}

void COLavlTreeBase::invariant(void) const
{
   testLinkage(RootNode);
   if (RootNode == NULL)
   {
      COLASSERT(NodeCount == 0);
   }
   else
   {
      COLindex count = 0;
      for (COLavlTreePlace curr = first(); curr != NULL; curr = next(curr))
      {
         COLASSERT(curr->Balance >= -1 && curr->Balance <= 1);
         count++;
      }
      COLASSERT(count == NodeCount);
   }
}

#endif
