#ifndef __TSM_HANDLER_STACK_H__
#define __TSM_HANDLER_STACK_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSMhandlerStack
//
// Description:
//
// TSMhandlerStack class.  This encapulates the stack for the TSM serialization
// framework.  The stack is implemented using a vector which doesn't resize when reduced in
// size - i.e. incrementing and popping the stack is intended to be an inexpensive operation -
// no mallocs here....
//
// This was in TXM.
//
// Author: Eliot Muir
// Date:   Monday, February 12th, 2007 @ 01:17:57 PM
//
//---------------------------------------------------------------------------

#include <COL/COLostream.h>

#include "TSMobjectHandler.h"

class TSMhandlerStackPrivate;

class TSMhandlerStack 
{
public:
   TSMhandlerStack();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~TSMhandlerStack();

   void setCurrentHandler(TSMobjectHandler Handler);
   TSMobjectHandler currentHandler() const;

   void setCurrentObject(void* pObject);
   void* currentObject() const;

   void* stackObject(int i) const;

   void init(TSMobjectHandler Handler, void* pObject);
   bool initialized() const;

   void push();  // push current handler and object on to the stack
   unsigned int pop();

   COLostream& errorStream();
   COLstring& errorString();

private:
   TSMobjectHandler CurrentTagHandler;
   void* pCurrentObject;
   unsigned int DepthOfStack;
   bool Initialized;
   COLstring ErrorString;
   COLostream ErrorStream;

   TSMhandlerStackPrivate* pMember;
   TSMhandlerStack(const TSMhandlerStack& Orig); // not allowed
   TSMhandlerStack& operator=(const TSMhandlerStack& Orig); // not allowed
};

#endif // end of defensive include
