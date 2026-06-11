//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBserialize
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, November 3rd, 2010 @ 11:43:03 AM
//
//---------------------------------------------------------------------------
#include "NOBprecomp.h"
#pragma hdrstop

#include "NOBserialize.h"
#include "NOBcomposite.h"
#include "NOBsimple.h"
#include "NOBlabels.h"

#include <NOD/NODplace.h>

#include <TBM/TBMwriter.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


static const COLstring EmptyValue;

static void NOBwriteDummyNode(const NODplace& DummyPlace, TSMwriter& Writer, NOBmemoizer& Memoizer);
static void NOBwriteNode(const NODplace& Place, TSMwriter& Writer, NOBmemoizer& Memoizer, bool WriteMemoizer = false);


// Max size of 64k is due to TBM (see TBMreader.h).
#define NOB_STRING_SIZE_LIMIT 64*1024
static void NOBwriteStringWithSizeCheck(TSMwriter& Writer, const TSMlabel& Label, const COLstring& Value){
   if (Value.size() > NOB_STRING_SIZE_LIMIT){
      COL_ERROR_STREAM_PLAIN("The following data is too large to cache: " << Value, COLerror::PreCondition);
   }
   Writer.addString(Label, Value);
}

static void NOBwriteMemoizedString(TSMwriter& Writer, const TSMlabel& Label, const COLstring& Value, NOBmemoizer& Memoizer){
   COLint64 Id = Memoizer.idFromValue(Value);
   if (Id > COL_INT_32_MAX){
      // This is more understandable than the real error.
      COL_ERROR_STREAM_PLAIN("Tree is too big to serialize.", COLerror::PreCondition);
   }
   Writer.addInt32(Label, Id);
}

static void NOBwriteSimpleNodeImpl(const NODplace& Place, TSMwriter& Writer, NOBmemoizer& Memoizer){
   Writer.addInt32(NOBflagsLabel, (COLint32)NOBgenerateSimpleFlags(Place));

   Writer.addInt32(NOBnodeTypeLabel,     Place.nodeType());
   Writer.addInt32(NOBprotocolTypeLabel, Place.protocolType());

   Memoizer.addProtocol(Place.protocolType(), Place.protocolName());
   Memoizer.addNodeType(Place.protocolType(), Place.nodeType(), Place.nodeTypeName());

   NOBwriteMemoizedString(Writer, NOBnameLabel, Place.name(), Memoizer);
}

static void NOBwriteCompositeNodeImpl(const NODplace& Place, TSMwriter& Writer, NOBmemoizer& Memoizer){
   Writer.addInt32(NOBflagsLabel, (COLint32)NOBgenerateCompositeFlags(Place));

   Writer.addInt32(NOBnodeTypeLabel,     Place.nodeType());
   Writer.addInt32(NOBprotocolTypeLabel, Place.protocolType());

   Memoizer.addProtocol(Place.protocolType(), Place.protocolName());
   Memoizer.addNodeType(Place.protocolType(), Place.nodeType(), Place.nodeTypeName());
   
   NOBwriteMemoizedString(Writer, NOBnameLabel, Place.name(), Memoizer);

   // Write children (or a dummy item).
   if (Place.canCreateDummyItem()){
      COLref<NODplace> pDummyItem = Place.createDummyItem();
      NOBwriteDummyNode(*pDummyItem, Writer, Memoizer);
   } else {
      int CountOfChild = Place.size();
      for (int i = 0; i < CountOfChild; i++){
         NOBwriteNode(Place.child(i), Writer, Memoizer);
      }
   }
}

static void NOBwriteDummyNode(const NODplace& DummyPlace, TSMwriter& Writer, NOBmemoizer& Memoizer){
   if (DummyPlace.isSimple()){
      Writer.beginObject(NOBdummySimpleNodeLabel);
      NOBwriteSimpleNodeImpl(DummyPlace, Writer, Memoizer);
      Writer.endObject(NOBsimpleNodeLabel);
   } else {
      Writer.beginObject(NOBdummyCompositeNodeLabel);
      NOBwriteCompositeNodeImpl(DummyPlace, Writer, Memoizer);
      Writer.endObject(NOBcompositeNodeLabel);
   }
}

static void NOBwriteMemoizer(const NOBmemoizer& Memoizer, TSMwriter& Writer){
   Writer.beginObject(NOBmemoizerLabel);
   for (COLhashmapPlace EntryPlace = Memoizer.first(); EntryPlace; EntryPlace = Memoizer.next(EntryPlace)){
      Writer.beginObject(NOBmemoizerEntryLabel);
      Writer.addInt32(NOBmemoizerEntryIdLabel, Memoizer.id(EntryPlace));
      Writer.addString(NOBmemoizerEntryValueLabel, Memoizer.value(EntryPlace));
      Writer.endObject(NOBmemoizerEntryLabel);
   }
   const COLhashmap<int, COLstring>& ProtocolLookup = Memoizer.protocolLookup();
   for (COLhashmapPlace ProtocolPlace = ProtocolLookup.first(); ProtocolPlace; ProtocolPlace = ProtocolLookup.next(ProtocolPlace)){
      Writer.beginObject(NOBmemoizerProtocolLabel);
      Writer.addInt32(NOBmemoizerEntryIdLabel, ProtocolLookup.key(ProtocolPlace));
      Writer.addString(NOBmemoizerEntryValueLabel, ProtocolLookup[ProtocolPlace]);
      Writer.endObject(NOBmemoizerProtocolLabel);
   }
   const COLhashmap<int, COLstring>& NodeTypeLookup = Memoizer.nodeTypeLookup();
   for (COLhashmapPlace NodeTypePlace = NodeTypeLookup.first(); NodeTypePlace; NodeTypePlace = NodeTypeLookup.next(NodeTypePlace)){
      Writer.beginObject(NOBmemoizerNodeTypeLabel);
      Writer.addInt32(NOBmemoizerEntryIdLabel, NodeTypeLookup.key(NodeTypePlace));
      Writer.addString(NOBmemoizerEntryValueLabel, NodeTypeLookup[NodeTypePlace]);
      Writer.endObject(NOBmemoizerNodeTypeLabel);
   }
   Writer.endObject(NOBmemoizerLabel);
}

static void NOBwriteNode(const NODplace& Place, TSMwriter& Writer, NOBmemoizer& Memoizer, bool WriteMemoizer){
   if (Place.isSimple()){
      if (Place.value().is_null()){
         Writer.beginObject(NOBsimpleEmptyNodeLabel);
         NOBwriteSimpleNodeImpl(Place, Writer, Memoizer);
         if (WriteMemoizer) NOBwriteMemoizer(Memoizer, Writer);
         Writer.endObject(NOBsimpleEmptyNodeLabel);
      } else {
         Writer.beginObject(NOBsimpleNodeLabel);
         NOBwriteSimpleNodeImpl(Place, Writer, Memoizer);
         // TODO - what about using the same memoizer lookup (or a different one) for values?
         NOBwriteStringWithSizeCheck(Writer, NOBvalueLabel, Place.value());
         if (WriteMemoizer) NOBwriteMemoizer(Memoizer, Writer);
         Writer.endObject(NOBsimpleNodeLabel);
      }
   } else {
      Writer.beginObject(NOBcompositeNodeLabel);
      NOBwriteCompositeNodeImpl(Place, Writer, Memoizer);
      if (WriteMemoizer) NOBwriteMemoizer(Memoizer, Writer);
      Writer.endObject(NOBcompositeNodeLabel);
   }
}

void NOBserialize(const NODplace& RootPlace, COLstring& Destination){
   COLsinkString Sink(Destination);
   TBMwriter Writer(Sink);
   NOBmemoizer Memoizer;
   NOBwriteNode(RootPlace, Writer, Memoizer, true);
}
