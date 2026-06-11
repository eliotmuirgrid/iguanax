#ifndef __TBM_BUFFERED_READER_H__
#define __TBM_BUFFERED_READER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TBMbufferedReader
//
// Description:
//
// TBMbufferedReader is one implementation of buffered reading for TBMwriter.  Unlike
// the raw TBMwriter this class does take ownership of the data it is given.  This is
// intended to be a convenient implementation that fits the needs of some applications.
//
// Whereas TBMwriter is the raw implementation that could be used by applications which
// need tighter control.
//
// Author: Eliot Muir
// Date:   Tuesday, February 13th, 2007 @ 01:51:31 PM
//
//---------------------------------------------------------------------------

#include "TBMresult.h"

class TSMhandlerStack;

class TBMbufferedReaderPrivate;

class TBMbufferedReader 
{
public:
   TBMbufferedReader();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~TBMbufferedReader();

   TBMresult write(const char* cpBuffer, COLuint32 SizeOfBuffer, COLuint32& ParsedUntil);

   // This is exposed so we can initialize it.
   TSMhandlerStack& handlerStack();

   void reset();
private:
   TBMbufferedReaderPrivate* pMember;
   TBMbufferedReader(const TBMbufferedReader& Orig); // not allowed
   TBMbufferedReader& operator=(const TBMbufferedReader& Orig); // not allowed
};

#endif // end of defensive include
