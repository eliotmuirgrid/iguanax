//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSMhandlerStack
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, February 12th, 2007 @ 01:17:57 PM
//
//---------------------------------------------------------------------------
#include "TSMprecomp.h"
#pragma hdrstop

#include "TSMhandlerStack.h"

#include <LEG/LEGvector.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

class TSMstackItem
{
public:
   TSMobjectHandler TagHandler;
   void*         pObject;
};

class TSMhandlerStackPrivate
{
public:
   LEGvector<TSMstackItem> Stack;
};

TSMhandlerStack::TSMhandlerStack()
: DepthOfStack(0),
  pCurrentObject(0),
  CurrentTagHandler(0),
  Initialized(false),
  ErrorStream(ErrorString)
{
   pMember = new TSMhandlerStackPrivate;
}

TSMhandlerStack::~TSMhandlerStack()
{
   delete pMember;
}

void TSMhandlerStack::setCurrentHandler(TSMobjectHandler Handler) {
   COL_METHOD(TSMhandlerStack::setCurrentHandler);
   COL_VAR(&Handler);
   COLPRECONDITION(Initialized);
   CurrentTagHandler = Handler; 
}

TSMobjectHandler TSMhandlerStack::currentHandler() const {
   COL_METHOD(TSMhandlerStack::currentHandler);
   COLPRECONDITION(Initialized);
   return CurrentTagHandler; 
}

void TSMhandlerStack::setCurrentObject(void* pObject) {
   COL_METHOD(TSMhandlerStack::setCurrentObject);
   COLPRECONDITION(Initialized);
   pCurrentObject = pObject; 
}

void* TSMhandlerStack::currentObject() const {
   COL_METHOD(TSMhandlerStack::currentObject);
   COLPRECONDITION(Initialized);
   return pCurrentObject; 
}

void TSMhandlerStack::init(TSMobjectHandler Handler, void* pObject) {
   COL_METHOD(TSMhandlerStack::init);
   Initialized = true;
   DepthOfStack = 0;

   if (pMember->Stack.size() == 0) {
      pMember->Stack.push_back();
   }
   CurrentTagHandler = Handler;
   pCurrentObject = pObject;
   pMember->Stack[DepthOfStack].TagHandler = Handler;
   pMember->Stack[DepthOfStack].pObject = pCurrentObject;
   ErrorString.clear();
}

bool TSMhandlerStack::initialized() const {
   return Initialized;
}

void TSMhandlerStack::push() {
   COL_METHOD(TSMhandlerStack::push);
   COLPRECONDITION(Initialized);
   // TODO some optimization is possible here...
   DepthOfStack++;
   if (DepthOfStack == pMember->Stack.size()) {
      pMember->Stack.push_back();
   }
   pMember->Stack[DepthOfStack].TagHandler = CurrentTagHandler;
   pMember->Stack[DepthOfStack].pObject = pCurrentObject;
}

unsigned int TSMhandlerStack::pop(){
   COL_METHOD(TSMhandlerStack::pop);
   COLPRECONDITION(Initialized);
   COLPRECONDITION(DepthOfStack > 0);
   DepthOfStack--;
   pCurrentObject = pMember->Stack[DepthOfStack].pObject;
   CurrentTagHandler = pMember->Stack[DepthOfStack].TagHandler;
   return DepthOfStack;
}

COLostream& TSMhandlerStack::errorStream() {
   return ErrorStream;
}

COLstring& TSMhandlerStack::errorString() {
   return ErrorString;
}

void* TSMhandlerStack::stackObject(int i) const {
   return pMember->Stack[i].pObject;
}
