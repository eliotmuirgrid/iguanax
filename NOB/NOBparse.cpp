//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBparse
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, November 3rd, 2010 @ 11:43:22 AM
//
//---------------------------------------------------------------------------
#include "NOBprecomp.h"
#pragma hdrstop

#include "NOBparse.h"
#include "NOBnode.h"
#include "NOBcomposite.h"
#include "NOBsimple.h"
#include "NOBlabels.h"

#include <TSM/TSMhandlerStack.h>
#include <TSM/TSMlabel.h>
#include <TSM/TSMattributeList.h>

#include <TBM/TBMreader.h>

#include <COL/COLauto.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static void NOBprocessCompositeAttributes(NOBcomposite& Node, const TSMattributeList& AttributeList){
   COLuint32 CountOfAttribute = AttributeList.countOfAttribute();
   for (COLuint32 i = 0; i < CountOfAttribute; i++){
      const TSMattribute& Attribute = AttributeList.attribute(i);
      const TSMlabel& Label = Attribute.name();
      const TSMvariant& Value = Attribute.value();

      switch(Attribute.name().id()){
      case NOB_FLAGS:              Node.setFlags((COLint8)Value.asInt32()); break;
      case NOB_NODE_TYPE:          Node.setNodeType(Value.asInt32()); break;
      case NOB_PROTOCOL_TYPE:      Node.setProtocolType(Value.asInt32()); break;
      case NOB_NAME:               Node.setName(Value.asInt32()); break;
      default:
         COL_WRN("Unexpected attribute for binary tree node.");
      }
   }
}

static void NOBprocessSimpleAttributes(NOBsimple& Node, const TSMattributeList& AttributeList){
   COLuint32 CountOfAttribute = AttributeList.countOfAttribute();
   for (COLuint32 i = 0; i < CountOfAttribute; i++){
      const TSMattribute& Attribute = AttributeList.attribute(i);
      const TSMlabel& Label = Attribute.name();
      const TSMvariant& Value = Attribute.value();

      switch(Attribute.name().id()){
      case NOB_FLAGS:              Node.setFlags((COLint8)Value.asInt32()); break;
      case NOB_NODE_TYPE:          Node.setNodeType(Value.asInt32()); break;
      case NOB_PROTOCOL_TYPE:      Node.setProtocolType(Value.asInt32()); break;
      case NOB_NAME:               Node.setName(Value.asInt32()); break;
      case NOB_VALUE:              Node.setCachedValue(Value.asString()); break;
      default:
         COL_WRN("Unexpected attribute for binary tree node.");
      }
   }
}

// Same as above, but no value handler.
static void NOBprocessSimpleEmptyAttributes(NOBsimpleEmpty& Node, const TSMattributeList& AttributeList){
   COLuint32 CountOfAttribute = AttributeList.countOfAttribute();
   for (COLuint32 i = 0; i < CountOfAttribute; i++){
      const TSMattribute& Attribute = AttributeList.attribute(i);
      const TSMlabel& Label = Attribute.name();
      const TSMvariant& Value = Attribute.value();

      switch(Attribute.name().id()){
      case NOB_FLAGS:              Node.setFlags((COLint8)Value.asInt32()); break;
      case NOB_NODE_TYPE:          Node.setNodeType(Value.asInt32()); break;
      case NOB_PROTOCOL_TYPE:      Node.setProtocolType(Value.asInt32()); break;
      case NOB_NAME:               Node.setName(Value.asInt32()); break;
      default:
         COL_WRN("Unexpected attribute for binary tree node.");
      }
   }
}

static void NOBlegacyProcessAttributes(NOBnode& Node, const TSMattributeList& AttributeList){
   COLuint32 CountOfAttribute = AttributeList.countOfAttribute();
   for (COLuint32 i = 0; i < CountOfAttribute; i++){
      const TSMattribute& Attribute = AttributeList.attribute(i);
      const TSMlabel& Label = Attribute.name();
      const TSMvariant& Value = Attribute.value();

      switch(Attribute.name().id()){
      case NOB_GRAMMAR_TYPE:            Node.setGrammarType((NODgrammarType)Value.asInt32()); break;
      case NOB_NODE_TYPE:               Node.setNodeType(Value.asInt32()); break;
      case NOB_PROTOCOL_TYPE:           Node.setProtocolType(Value.asInt32()); break;
      case NOB_NODE_TYPE_NAME:          Node.setNodeTypeName(Value.asString()); break;
      case NOB_PROTOCOL_TYPE_NAME:      Node.setProtocolName(Value.asString()); break;
      case NOB_NAME:                    Node.setName(Value.asString()); break;
      case NOB_IS_NAMED:                Node.setIsNamed(Value.asBoolean()); break;
      case NOB_IS_NULL:                 Node.setIsNull(Value.asBoolean()); break;
      case NOB_VALUE:                   Node.setCachedValue(Value.asString()); break;
      case NOB_SHOULD_ESCAPE_VALUE:     Node.setShouldEscapeValue(Value.asBoolean()); break;
      case NOB_IS_HOMOGENEOUS:          Node.setIsHomogeneous(Value.asBoolean()); break;
      case NOB_FAVOR_INDEX_BY_NAME:     Node.setFavorIndexByName(Value.asBoolean()); break;
      case NOB_GRACEFUL_NO_SUCH_MEMBER: Node.setGracefulNoSuchMember(Value.asBoolean()); break;
      default:
         COL_WRN("Unexpected attribute for binary tree node.");
      }
   }
}

static void NOBmemoizerEntryHandler(TSMhandlerStack& HandlerStack, const TSMlabel& Label, const TSMattributeList& AttributeList){
   NOBmemoizer* pMemoizer = (NOBmemoizer*)HandlerStack.currentObject();
   COLuint32 CountOfAttribute = AttributeList.countOfAttribute();
   COLstring Value;

   COLint32 Id = -1;
   for (COLuint32 i = 0; i < CountOfAttribute; i++){
      const TSMattribute& Attribute = AttributeList.attribute(i);
      if (Attribute.name() == NOBmemoizerEntryIdLabel){
         Id = Attribute.value().asInt32();
      } else {
         Value = Attribute.value().asString();
      }
   }

   if (Label == NOBmemoizerEntryLabel){
      pMemoizer->add(Id, Value);
   } else if (Label == NOBmemoizerNodeTypeLabel){
      pMemoizer->addRawNodeType(Id, Value);
   } else if (Label == NOBmemoizerProtocolLabel){
      pMemoizer->addProtocol(Id, Value);
   } else {
      COL_WRN("Unexpected child found for memoizer: " << Label);
   }
}

static void NOBparseMemoizer(TSMhandlerStack& HandlerStack, NOBmemoizer* pMemoizer){
   HandlerStack.setCurrentObject(pMemoizer);
   HandlerStack.setCurrentHandler(NOBmemoizerEntryHandler);
}

static void NOBcompositeChildNodeHandler(TSMhandlerStack& HandlerStack, const TSMlabel& Label, const TSMattributeList& AttributeList){
   NOBcomposite* pParent = (NOBcomposite*)HandlerStack.currentObject();
   NODplace* pChild = NULL;
   if (Label == NOBcompositeNodeLabel){
      NOBcomposite* pComposite = new NOBcomposite(pParent->memoizer());
      pChild = pParent->addChild(pComposite);
      NOBprocessCompositeAttributes(*pComposite, AttributeList);
   } else if (Label == NOBsimpleNodeLabel){
      NOBsimple* pSimple = new NOBsimple(pParent->memoizer());
      pChild = pParent->addChild(pSimple);
      NOBprocessSimpleAttributes(*pSimple, AttributeList);
   } else if (Label == NOBsimpleEmptyNodeLabel){
      NOBsimpleEmpty* pSimple = new NOBsimpleEmpty(pParent->memoizer());
      pChild = pParent->addChild(pSimple);
      NOBprocessSimpleEmptyAttributes(*pSimple, AttributeList);
   } else if (Label == NOBdummyCompositeNodeLabel){
      NOBcomposite* pDummy = new NOBcomposite(pParent->memoizer());
      pChild = pParent->initDummyItem(pDummy);
      NOBprocessCompositeAttributes(*pDummy, AttributeList);
   } else if (Label == NOBdummySimpleNodeLabel){
      NOBsimpleEmpty* pDummy = new NOBsimpleEmpty(pParent->memoizer());
      pChild = pParent->initDummyItem(pDummy);
      NOBprocessSimpleEmptyAttributes(*pDummy, AttributeList);
   } else if (Label == NOBmemoizerLabel){
      NOBparseMemoizer(HandlerStack, pParent->memoizer());
   }
   if (pChild){
      HandlerStack.setCurrentObject(pChild);
   }
}

static void NOBsimpleChildNodeHandler(TSMhandlerStack& HandlerStack, const TSMlabel& Label, const TSMattributeList& AttributeList){
   NOBsimple* pNode = (NOBsimple*)HandlerStack.currentObject();
   if (Label == NOBmemoizerLabel){
      NOBparseMemoizer(HandlerStack, pNode->memoizer());
   } else {
      COL_WRN("Unexpected child found for simple node: " << Label);
   }
}

static void NOBlegacyChildNodeHandler(TSMhandlerStack& HandlerStack, const TSMlabel& Label, const TSMattributeList& AttributeList){
   NOBnode* pParent = (NOBnode*)HandlerStack.currentObject();
   NOBnode* pChild = NULL;
   if (Label == NOBlegacyNodeLabel){
      pChild = pParent->addChild();
   } else if (Label == NOBlegacyDummyItemLabel){
      pChild = pParent->initDummyItem();
   }
   COLASSERT(pChild);
   NOBlegacyProcessAttributes(*pChild, AttributeList);

   HandlerStack.setCurrentObject(pChild);
}

static void NOBrootHandler(TSMhandlerStack& HandlerStack, const TSMlabel& Label, const TSMattributeList& AttributeList){
   COLauto<NODplace>& pRootOwner = *((COLauto<NODplace>*)HandlerStack.currentObject());
   if (Label == NOBcompositeNodeLabel){
      NOBcomposite* pRoot = new NOBcomposite(new NOBmemoizer());
      pRootOwner = pRoot;
      HandlerStack.setCurrentObject(pRoot);
      NOBprocessCompositeAttributes(*pRoot, AttributeList);
      HandlerStack.setCurrentHandler(&NOBcompositeChildNodeHandler);
   } else if (Label == NOBsimpleNodeLabel){
      NOBsimple* pRoot = new NOBsimple(new NOBmemoizer());
      pRootOwner = pRoot;
      HandlerStack.setCurrentObject(pRoot);
      NOBprocessSimpleAttributes(*pRoot, AttributeList);
      HandlerStack.setCurrentHandler(&NOBsimpleChildNodeHandler);
   } else if (Label == NOBsimpleEmptyNodeLabel){
      NOBsimpleEmpty* pRoot = new NOBsimpleEmpty(new NOBmemoizer());
      pRootOwner = pRoot;
      HandlerStack.setCurrentObject(pRoot);
      NOBprocessSimpleEmptyAttributes(*pRoot, AttributeList);
      HandlerStack.setCurrentHandler(&NOBsimpleChildNodeHandler);
   } else if (Label == NOBlegacyNodeLabel){
      NOBnode* pRoot = new NOBnode();
      pRootOwner = pRoot;
      HandlerStack.setCurrentObject(pRoot);
      NOBlegacyProcessAttributes(*pRoot, AttributeList);
      HandlerStack.setCurrentHandler(&NOBlegacyChildNodeHandler);
   } else {
      COL_ERROR_STREAM("Unrecognized label: " << Label, COLerror::ParsingError);
   }
}

NODplace* NOBparse(COLstring& Source){
   TBMreader Reader;

   COLauto<NODplace> pRoot;
   Reader.handlerStack().init(&NOBrootHandler, &pRoot);

   COLuint32 ParsedUntil = 0, AmountNeeded = Source.size();
   Reader.parse(Source.c_str(), AmountNeeded, ParsedUntil, AmountNeeded);
   COLASSERT(AmountNeeded == 0);

   return pRoot.release();
}
