// ---------------------------------------------------------------------------
// Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAXtoken
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 25 February 2026 - 12:26PM
// ---------------------------------------------------------------------------

#include "LUAXtoken.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

LUAXtoken::LUAXtoken(int iToken, size_t iEnd, size_t iSize, int iLine, LUAXtoken* ipNext) : Token(iToken), TokenEnd(iEnd), TokenSize(iSize), Line(iLine), pNext(ipNext){
   COL_METHOD(LUAXtoken::LUAXtoken);
}

LUAXtoken::~LUAXtoken(){
   COL_METHOD(LUAXtoken::~LUAXtoken);
   LUAXtoken* pCurrent = pNext;
   while (pCurrent){
      LUAXtoken* pCurrentNext = pCurrent->pNext;
      pCurrent->pNext = NULL;
      delete pCurrent;
      pCurrent = pCurrentNext;
   }
}
