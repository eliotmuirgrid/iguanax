//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PANICmode
//
// Description:
//
// Implementation.
// 
// Author: Vismay Shah
//---------------------------------------------------------------------------
#include "PANICmode.h"

#include <COL/COLstring.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void PANICmode::setPanicMode(PANICerrorCode ErrorCode, const COLstring& ErrorDescription){
   COL_METHOD(PANICmode::setPanicMode);
   //we check for presence of the key to make sure we don't do this multiple times
   //in case we got the event again
   if (!m_PanicErrorCodesToError.find(ErrorCode)){
      m_PanicErrorCodesToError[ErrorCode] = ErrorDescription;
      // DBDaddServerLog(*pOwner, DBQ_ERROR_MESSAGE, ErrorDescription + "  Stopping all channels."); // Separate concern? Code calling setPanicMode() should stop channels.
      // TODO - stop all channels
   }
}

bool PANICmode::inPanicMode() const{
   COL_METHOD(PANICmode::inPanicMode);
   return m_PanicErrorCodesToError.size() != 0;
}

COLstring PANICmode::panicErrorStrings() const{
   COL_METHOD(PANICmode::panicErrorStrings);
   COLstring OutString;
   COLostream OutStream(OutString);
   for (COLhashmapPlace Place = m_PanicErrorCodesToError.first(); Place; Place = m_PanicErrorCodesToError.next(Place)){
      OutStream << m_PanicErrorCodesToError[Place] << newline;
   }
   return OutString;
}

void PANICmodeDueToMoveNextError(PANICmode* pPanicMode, const COLstring& ErrorMessage) {
   COL_FUNCTION(PANICmodeDueToMoveNextError);
   pPanicMode->setPanicMode(PANIC_LOG_MOVE_NEXT_FAIL, ErrorMessage);
}

void PANICmodeDueToLogValidateError(PANICmode* pPanicMode, const COLstring& ErrorMessage) {
   COL_FUNCTION(PANICmodeDueToLogValidateError);
   pPanicMode->setPanicMode(PANIC_LOG_VALIDATE_FAIL, ErrorMessage);
}

void PANICmodeDueToLogInitError(PANICmode* pPanicMode, const COLstring& ErrorMessage) {
   COL_FUNCTION(PANICmodeDueToLogInitError);
   pPanicMode->setPanicMode(PANIC_LOG_INIT_FAIL, ErrorMessage);
}

void PANICmodeDueToInvalidLogLocker(PANICmode* pPanicMode, const COLstring& ErrorMessage) {
   COL_FUNCTION(PANICmodeDueToInvalidLogLocker);
   pPanicMode->setPanicMode(PANIC_LOG_INVALID_LOG_LOCKER, ErrorMessage);
}
   