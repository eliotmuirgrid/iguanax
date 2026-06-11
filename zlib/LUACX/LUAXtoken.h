#ifndef __LUAX_TOKEN_H__
#define __LUAX_TOKEN_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAXtoken
//
// Description:
//
// Token class
//
// Author: Eliot Muir 
// Date:   Wednesday 25 February 2026 - 12:26PM
// ---------------------------------------------------------------------------

#include <stddef.h>

class LUAXtoken{
public:
   LUAXtoken(int iToken, size_t iEnd, size_t iSize, int iLine, LUAXtoken* ipNext);

   ~LUAXtoken();

   int Token;
   size_t TokenEnd;
   size_t TokenSize;
   int Line;
   LUAXtoken* pNext;
};

#endif // end of defensive include
