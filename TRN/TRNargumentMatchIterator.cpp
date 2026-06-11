// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentMatchIterator
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 02 May 2023 - 09:13AM
// ---------------------------------------------------------------------------

#include <TRN/TRNargumentMatchIterator.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

TRNargumentMatchIterator::TRNargumentMatchIterator(COLarray<TRNargRequest>& RequestList, const COLstring& FunctionAddress, bool IsReturn)
 : m_RequestList(RequestList), m_FunctionAddress(FunctionAddress), m_IsReturn(IsReturn),
      m_CurrentIndex(-1), m_RequestListSize(RequestList.size()){
   COL_METHOD(TRNargumentMatchIterator::TRNargumentMatchIterator);
}

TRNargRequest* TRNargumentMatchIteratorFindNext(TRNargumentMatchIterator* pIterator){
   COL_FUNCTION(TRNargumentMatchIteratorFindNext);
   while (++pIterator->m_CurrentIndex < pIterator->m_RequestListSize){
      TRNargRequest& Request = pIterator->m_RequestList[pIterator->m_CurrentIndex];
      COL_TRC("Request:");
      COL_VAR3(Request.m_Id, Request.m_FunctionAddress, Request.m_IsReturn);
      COL_TRC("Iterator:");
      COL_VAR2(pIterator->m_FunctionAddress, pIterator->m_IsReturn);
      if (Request.m_FunctionAddress == pIterator->m_FunctionAddress && Request.m_IsReturn == pIterator->m_IsReturn && Request.m_VarResult.isNull()){
         return &Request;
      }
   }
   return NULL;
}