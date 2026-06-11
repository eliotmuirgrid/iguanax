#ifndef __TRN_FRAME_H__
#define __TRN_FRAME_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNframe
//
// Description:
//
// TRNframe class representing a lua stack frame.
//
// Author: Vismay Shah 
// Date:   Wednesday 03 May 2023 - 01:45PM
// ---------------------------------------------------------------------------
#include <TRN/TRNfunction.h>
#include <TRN/TRNscope.h>

class TRNframe{
public:
   TRNframe();

   int m_InternalLevel;
   int m_FirstLine;
   int m_CurrentLine;
   TRNfunctionCall m_Current;
   int m_CountOfParam;
   bool m_ReceivedParams;
   // Map for counting the number of times this function has called others.
   COLhashmap<COLstring, int> m_CallCountMap;
   const char* m_pSource; // the "source" containing the function, as provided by the Lua debug API.
private:
   // avoid unnecessary copies
   TRNframe(const TRNframe& Orig); // now allowed
   TRNframe& operator=(const TRNframe& Orig); // not allowed
};

void TRNframeInit(TRNframe* pFrame, int InternalLevel, int FirstLine, const COLstring& FunctionName, TRNfunction* pFunction, int CountOfParamGuess, const char* pSource);


#endif // end of defensive include
