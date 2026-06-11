//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBcopy
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Monday, September 19th, 2011 @ 02:23:46 PM
//
//---------------------------------------------------------------------------
#include "NOBprecomp.h"
#pragma hdrstop

#include "NOBcopy.h"
#include "NOBmemoizer.h"
#include "NOBsimple.h"
#include "NOBsimpleEmpty.h"
#include "NOBcomposite.h"

#include <COL/COLauto.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static NODplace* NOBcopyImpl(const NODplace& Original, NOBmemoizer* pMemoizer);


static void NOBcopySimple(NOBsimpleEmpty& Copy, const NODplace& Original, NOBmemoizer* pMemoizer){
   // Properties
   int NodeType = Original.nodeType(), ProtocolType = Original.protocolType();
   Copy.setFlags(NOBgenerateSimpleFlags(Original));
   Copy.setNodeType(NodeType);
   Copy.setProtocolType(ProtocolType);
   Copy.setName(Original.name());

   // Type info
   pMemoizer->addProtocol(ProtocolType, Original.protocolName());
   pMemoizer->addNodeType(ProtocolType, NodeType, Original.nodeTypeName());
}

static void NOBcopyComposite(NOBcomposite& Copy, const NODplace& Original, NOBmemoizer* pMemoizer){
   // Properties
   int NodeType = Original.nodeType(), ProtocolType = Original.protocolType();
   Copy.setFlags(NOBgenerateCompositeFlags(Original));
   Copy.setNodeType(NodeType);
   Copy.setProtocolType(ProtocolType);
   Copy.setName(Original.name());

   // Type info
   pMemoizer->addProtocol(ProtocolType, Original.protocolName());
   pMemoizer->addNodeType(ProtocolType, NodeType, Original.nodeTypeName());

   // Children
   for (int i = 0, n = Original.size(); i < n; i++){
      Copy.addChild(NOBcopyImpl(Original.child(i), pMemoizer));
   }

   // Dummy item
   if (Original.canCreateDummyItem()){
      COLref<NODplace> pDummyItem = Original.createDummyItem();
      Copy.initDummyItem(NOBcopyImpl(*pDummyItem, pMemoizer));
   }
}

static NODplace* NOBcopyImpl(const NODplace& Original, NOBmemoizer* pMemoizer){
   if (Original.isSimple()){
      if (Original.value().is_null()){
         COLauto<NOBsimpleEmpty> pSimple = new NOBsimpleEmpty(pMemoizer);
         NOBcopySimple(*pSimple, Original, pMemoizer);
         return pSimple.release();
      } else {
         COLauto<NOBsimple> pSimple = new NOBsimple(pMemoizer);
         NOBcopySimple(*pSimple, Original, pMemoizer);
         pSimple->setCachedValue(Original.value());
         return pSimple.release();
      }
   } else {
      COLauto<NOBcomposite> pComposite = new NOBcomposite(pMemoizer);
      NOBcopyComposite(*pComposite, Original, pMemoizer);
      return pComposite.release();
   }
}

NODplace* NOBcopy(const NODplace& Original, const char* pKeyName){
   COLref<NOBmemoizer> pMemoizer = new NOBmemoizer();
   if (pKeyName) pMemoizer->setKeyName(pKeyName);
   return NOBcopyImpl(Original, pMemoizer.get());
}
