//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODoperation
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Thursday, May 13th, 2010 @ 02:25:46 PM
//
//---------------------------------------------------------------------------
#include "NODprecomp.h"
#pragma hdrstop

#include "NODoperation.h"
#include "NODplace.h"
#include "NODaddress.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

/*COLstring NODerrorCodeAsString(NODerrorCode Code)
{
   switch(Code)
   {
   case NOD_ESUCCESS:            return "Succeeded";
   case NOD_NO_SUCH_MEMBER:      return "No such member";
   case NOD_NOT_COMPLEX_TYPE:    return "Not a complex type";
   case NOD_IS_LEAF:             return "Is a simple type.";
   case NOD_IS_VECTOR:           return "Attempted to set a property on a vector.";
   case NOD_INDEX_OUT_OF_BOUNDS: return "Index out of bounds.";
   case NOD_IS_READ_ONLY:        return "Attempt to modify a read-only tree.";
   default:
      COLASSERT(1==0);
      return COLstring();
   }
}*/

static bool NODgrammarNodesAreEqual(const NODplace& GrammarA, const NODplace& GrammarB)
{
   return GrammarA.protocolType() == GrammarB.protocolType() &&
          GrammarA.nodeType()     == GrammarB.nodeType()     &&
          GrammarA.grammarType()  == GrammarB.grammarType()  &&
          (GrammarA.grammarType() != NOD_COMPLEX || GrammarA.size() == GrammarB.size());
   //&&
   //       GrammarA.name()         == GrammarB.name();
}

static bool NODgrammarTreesAreEqual(const NODplace& GrammarA, const NODplace& GrammarB)
{
   if (!NODgrammarNodesAreEqual(GrammarA, GrammarB))
   {
      return false;
   }

   if (GrammarA.isComplex())
   {
      int CountOfChild = GrammarA.size();
      for (int ChildIndex = 0; ChildIndex < CountOfChild; ChildIndex++){
         if (!NODgrammarTreesAreEqual(GrammarA.child(ChildIndex), GrammarB.child(ChildIndex))) {
            return false;
         }
      }
   }
   else if (GrammarA.isVector() && GrammarA.isHomogeneous())
   {
      COLref<NODplace> pGrammarADummyItem = GrammarA.createDummyItem();
      COLref<NODplace> pGrammarBDummyItem = GrammarB.createDummyItem();
      return NODgrammarTreesAreEqual(*pGrammarADummyItem, *pGrammarBDummyItem);
   }
   // We can't really say whether or not heterogeneous vectors share the same "grammar",
   // since a variety of grammars is valid.

   return true;
}

void NODclearRange(NODplace& Place, int Start, int End){
   int MaxEnd = Place.size();
   if (End == -1){
      End = MaxEnd;
   }

   if (Place.isSimple()){
      COL_ERROR_STREAM_PLAIN("This is a leaf node without children.", NOD_IS_LEAF);
   } else if (Start < 0 || Start >= MaxEnd || End < 0 || End > MaxEnd){
      // The below message is more accurate for the Lua API, from which we decrement Start.
      COL_ERROR_STREAM_PLAIN(COL_T("The range ") << (Start+1) << COL_T(" to ") << End << COL_T(" is out of bounds."), NOD_INDEX_OUT_OF_BOUNDS);
   } else if (Start >= End){
      // The below message is more accurate for the Lua API, from which we decrement Start.
      COL_ERROR_STREAM_PLAIN("Start of range must be less than or equal to end of range.", NOD_INDEX_OUT_OF_BOUNDS);
   }
   else if (Place.isComplex()){
      for (int ChildIndex = Start; ChildIndex < End; ChildIndex++){
         Place.nullify(ChildIndex);
      }
   }else if (Place.isVector()){
      for (int ItemIndex = End-1; ItemIndex >= Start; ItemIndex--){
         Place.remove(ItemIndex);
      }
   }
}

// Returns false if the node trees do not match (so a valid copy was not possible).
static bool NODcopyNodeImpl(const NODplace& Source, NODplace& Destination){
   COL_FUNCTION(NODcopyNodeImpl);
   if (!NODgrammarNodesAreEqual(Source, Destination)){
      return false;
   }

   switch (Source.grammarType()){
   case NOD_SIMPLE:
      COL_TRC("Copying simple node.");
      if (!Source.isNull()){
         COL_TRC("Setting " << Source.value());
         Destination.setValue(Source.value());
      } else {
         COL_TRC("Source is null ");
         Destination.setIsNull(true);
         Destination.clear();
      }
      break;
   case NOD_VECTOR:
      if (Source.isHomogeneous())
      {
         if (Destination.size() > Source.size())
         {
            NODclearRange(Destination, Source.size());
         }
         int CountOfItem = Source.size();
         for (int ItemIndex = 0; ItemIndex < CountOfItem; ItemIndex++)
         {
            if (!NODcopyNodeImpl(Source.child(ItemIndex), Destination.child(ItemIndex)))
            {
               return false;
            }
         }
      }
      else // heterogeneous vector
      {
         Destination.clear();
         int CountOfItem = Source.size();
         for (int ItemIndex = 0; ItemIndex < CountOfItem; ItemIndex++)
         {
            const NODplace& SourceChild = Source.child(ItemIndex);
            NODplace& DestinationChild = Destination.add(SourceChild.nodeTypeName().c_str(), NULL);
            if (!NODcopyNodeImpl(SourceChild, DestinationChild))
            {
               return false;
            }
         }
      }
      break;
   case NOD_COMPLEX:{
         COL_TRC("Copying complex node.");
         int CountOfChild = Source.size();
         for (int ChildIndex = 0; ChildIndex < CountOfChild; ChildIndex++)
         {
            const NODplace& SourceChild = Source.child(ChildIndex);

            if (!SourceChild.isNull())
            {
               NODplace& DestinationChild = Destination.child(ChildIndex);
               if (!NODcopyNodeImpl(SourceChild, DestinationChild)) {
                  return false;
               }
            }
            else
            {
               Destination.nullify(ChildIndex);
               // We use NODgrammarTreesAreEqual to iterate through the trees which keeps
               // the destination tree as a const, so it is not modified.
               const NODplace& DestinationChild = ((const NODplace&)Destination).child(ChildIndex);
               if (!NODgrammarTreesAreEqual(SourceChild, DestinationChild)) {
                  return false;
               }
            }
         }
      }
      break;
   }

   return true;
}

// Returns false if a simple assignment is not possible.  A simple assignment is only
// possible if the only leaf in the tree with a value is the first leaf.
static bool NODdoSimpleAssignment(const NODplace& Source, NODplace& Destination)
{
   if (!Source.isSimple() || !Destination.isSimple())
   {
      return false;
   }
   else if (Source.isNull())
   {
      Destination.clear();
      return true;
   }
   else
   {
      Destination.setValue(Source.value());
      return true;
   }
}

void NODcopyNode(const NODplace& Source, NODplace& Destination){
   COL_FUNCTION(NODcopyNode);
   if (!NODdoSimpleAssignment(Source, Destination)) {
      if (&Source == &Destination){
         COL_ERROR_STREAM_PLAIN("Left and right side refer to the same tree - " << Destination.name(), COLerror::PreCondition);
      } else if (!NODcopyNodeImpl(Source, Destination)){
         COL_ERROR_STREAM_PLAIN("Left tree " << Destination.name() << " is not compatible with right tree " << Source.name(), COLerror::PreCondition);
      }
   }
}

static void NODmapTreeImpl(const NODplace& Source, NODplace& Destination){
   COL_FUNCTION(NODmapTreeImpl);
   if (Source.isNull()) return;
   if (Source.isSimple()){
      if (Destination.isSimple()){
         COL_TRC("Source and Destination are both Simple");
         Destination.setValue(Source.value());
      } else if ((Destination.isVector() && Destination.isHomogeneous()) || Destination.size()){
         COL_TRC("Source is Simple, Destination is a homogeneous vector and/or has at least one child.");
         NODplace& DestinationChild = Destination.child(0);
         NODmapTreeImpl(Source, DestinationChild);
      }
   } else if (Destination.isSimple()){ // (and Source is not Simple)
      if (Source.size()){
         COL_TRC("Destination is Simple, Source has at least one child.");
         NODmapTreeImpl(Source.child(0), Destination);
      }
   }else if (Source.isVector()){
      if (Destination.isVector()){
         COL_TRC("Source and Destination are Vectors");
         int CountOfItem = Source.size();
         for (int i = 0; i < CountOfItem; i++){
            NODmapTreeImpl(Source.child(i), Destination.child(i));
         }
      } else if (Source.size()){
         COL_TRC("Source is Vector with at least one item, Destination is Complex.");
         // Just map the first item in the source vector.
         NODmapTreeImpl(Source.child(0), Destination);
      }
   }else if (Destination.grammarType() == NOD_VECTOR){ // (and Source is not a vector)
      // Destination is Vector, Source is Complex.
      // Just map to the first item in the destination vector.
      if (Destination.isHomogeneous() || Destination.size())
      {
         NODmapTreeImpl(Source, Destination.child(0));
      }
   }else{
      COL_TRC("Source and Destination are both Complex.");
      int CountOfMember = COL_MIN(Source.size(), Destination.size());
      COL_VAR(CountOfMember);
      for (int i = 0; i < CountOfMember; i++){
         const NODplace& SourceChild = Source.child(i);
         if (!SourceChild.isNull()){
            NODmapTreeImpl(SourceChild, Destination.child(i));
         }
      }
   }
}

void NODmapTree(const NODplace& Source, NODplace& Destination){
   COL_FUNCTION(NODmapTree);
   if (&Source == &Destination){
      COL_ERROR_STREAM_PLAIN("Source and destination refer to the same tree.", COLerror::PreCondition);
   }
   COL_VAR2(&Source, &Destination);

   NODmapTreeImpl(Source, Destination);
}

void NODmapRange(const NODplace& Source, NODplace& Destination, int Start, int End)
{
   int MaxEnd = COL_MIN(Source.size(), Destination.size());
   if (End == -1)
   {
      End = MaxEnd;
   }

   if (Source.isSimple() || Destination.isSimple())
   {
      COL_ERROR_STREAM_PLAIN("Neither source nor destination can be leaf nodes.", NOD_IS_LEAF);
   }
   else if (&Source == &Destination)
   {
      COL_ERROR_STREAM_PLAIN("Source and destination refer to the same tree.", COLerror::PreCondition);
   }
   else if (Start < 0 || Start >= MaxEnd || End < 0 || End > MaxEnd)
   {
      // The below message is more accurate for the Lua API, from which we decrement Start.
      COL_ERROR_STREAM_PLAIN(COL_T("The range ") << (Start+1) << COL_T(" to ") << End << COL_T(" is out of bounds."), NOD_INDEX_OUT_OF_BOUNDS);
   }
   else if (Start >= End)
   {
      // The below message is more accurate for the Lua API, from which we decrement Start.
      COL_ERROR_STREAM_PLAIN("Start of range must be less than or equal to end of range.", NOD_INDEX_OUT_OF_BOUNDS);
   }

   for (int ChildIndex = Start; ChildIndex < End; ChildIndex++)
   {
      const NODplace& SourceChild = Source.child(ChildIndex);
      if (!SourceChild.isNull()){
         NODmapTree(SourceChild, Destination.child(ChildIndex));
      }
   }
}

void NODclearChild(NODplace& Current, int ChildIndex)
{
   if (Current.isVector())
   {
      if (ChildIndex < 0 || ChildIndex >= Current.size())
      {
         COL_ERROR_STREAM_PLAIN("Index out of bounds.", NOD_INDEX_OUT_OF_BOUNDS);
      }
      Current.remove(ChildIndex);
   }
   else if (Current.isComplex())
   {
      if (ChildIndex < 0 || ChildIndex >= Current.size())
      {
         COL_ERROR_STREAM_PLAIN("Index out of bounds.", NOD_INDEX_OUT_OF_BOUNDS);
      }
      Current.nullify(ChildIndex);
   }
   else
   {
      COL_ERROR_STREAM_PLAIN("This is a leaf node without children.", NOD_IS_LEAF);
   }
}

int NODcountOfNamedChildRepeat(const NODplace& Place, const COLstring& ChildName){
   if (Place.isSimple()){
      COL_ERROR_STREAM_PLAIN(COL_T("This is a leaf node without children."), NOD_IS_LEAF);
   } else if (Place.isVector()){
      COL_ERROR_STREAM_PLAIN(COL_T("This node does not have named members."), NOD_NOT_COMPLEX_TYPE);
   }

   int CurrentIndex = Place.childIndex(ChildName); // returns the index of the first match
   if (CurrentIndex == -1){
      return 0;
   }
   int CountOfRepeat = 1;
   int Size = Place.size();
   for (CurrentIndex++; CurrentIndex < Size; CurrentIndex++){
      const NODplace& Child = Place.child(CurrentIndex);
      if (Child.isNamed() && Child.name() == ChildName) CountOfRepeat++;
   }

   return CountOfRepeat;
}

int NODgetNamedChildRepeat(const NODplace& Place, const COLstring& ChildName, int RepeatIndex){
   if (Place.isSimple()){
      COL_ERROR_STREAM_PLAIN(COL_T("This is a leaf node without children."), NOD_IS_LEAF);
   } else if (Place.isVector()){
      COL_ERROR_STREAM_PLAIN(COL_T("This node does not have named members."), NOD_NOT_COMPLEX_TYPE);
   }

   int CurrentIndex = Place.childIndex(ChildName); // returns the index of the first match
   if (CurrentIndex == -1){
      COL_ERROR_STREAM_PLAIN('\'' << ChildName << COL_T("' member does not exist. ") << NODmemberList(Place), NOD_NO_SUCH_MEMBER);
   }
   int RepeatsLeft = RepeatIndex;
   int Size = Place.size();
   while (RepeatsLeft > 0 && ++CurrentIndex < Size){
      const NODplace& Child = Place.child(CurrentIndex);
      if (Child.isNamed() && Child.name() == ChildName) RepeatsLeft--;
   }
   if (CurrentIndex == Size){
      COL_ERROR_STREAM_PLAIN("Index " << RepeatIndex << " is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS);
   }
   return CurrentIndex;
}
