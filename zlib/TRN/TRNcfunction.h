#ifndef __TRN_CFUNCTION_H__
#define __TRN_CFUNCTION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNcfunction
//
// Description:
//
// TRNcfunction - data for a c function.
//
// Author: Eliot Muir
// Date:   Tuesday, November 23rd, 2010 @ 09:33:25 AM
//---------------------------------------------------------------------------
#include <COL/COLstring.h>

class TRNcfunction{
public:
   TRNcfunction(const COLstring& Name);
   ~TRNcfunction();

   // For identifying the function which is wrapped.
   enum eFunctionId{
      UNKNOWN,
      PCALL,
      ERROR
   };

   COLstring m_Name;
   int m_Calls;
   eFunctionId m_FunctionId;
private:
   TRNcfunction(const TRNcfunction& Orig); // not allowed
   TRNcfunction& operator=(const TRNcfunction& Orig); // not allowed
};

COLstring TRNcFunctionDisplayName   (TRNcfunction* cFunction);
int       TRNcFunctionIncrementCalls(TRNcfunction* pFunction);

#endif // end of defensive include
