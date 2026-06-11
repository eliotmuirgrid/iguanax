#ifndef __TRN_ARGUMENT_MATCH_ITERATOR_H__
#define __TRN_ARGUMENT_MATCH_ITERATOR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentMatchIterator
//
// Description:
//
// An iterator to find all full argument requests matching a call or return event.
// This is because the user could be viewing more than 1 parameter or return value for a
// given function call.
//
// Author: Vismay Shah 
// Date:   Tuesday 02 May 2023 - 09:13AM
// ---------------------------------------------------------------------------
#include <TRN/TRNargumentRequest.h>

class TRNargumentMatchIterator{
public:
   TRNargumentMatchIterator(COLarray<TRNargRequest>& RequestList, const COLstring& FunctionAddress, bool IsReturn);

   COLarray<TRNargRequest>& m_RequestList;
   const COLstring& m_FunctionAddress;
   bool m_IsReturn;
   int m_CurrentIndex;
   int m_RequestListSize;
};

TRNargRequest* TRNargumentMatchIteratorFindNext(TRNargumentMatchIterator* pIterator);

#endif // end of defensive include