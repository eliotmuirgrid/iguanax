#ifndef __PANIC_MODE_H__
#define __PANIC_MODE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PANICmode
//
// Description:
//
// This class handles Iguana's panic mode.
// 
// Author: Vismay Shah
//---------------------------------------------------------------------------

#include <COL/COLhashmap.h>
#include <COL/COLstring.h>

enum PANICerrorCode{
   PANIC_LOG_COMMIT_FAIL,
   PANIC_LOG_MOVE_NEXT_FAIL,
   PANIC_LOG_VALIDATE_FAIL,
   PANIC_LOG_INIT_FAIL,
   PANIC_LOG_INVALID_LOG_LOCKER
};

class PANICmode{
public:
   PANICmode(){};
   ~PANICmode(){};

   void setPanicMode(PANICerrorCode ErrorCode, const COLstring& ErrorDescription);
   bool inPanicMode() const;
   COLstring panicErrorStrings() const;
private:
   COLhashmap<COLuint32, COLstring> m_PanicErrorCodesToError;
};

void PANICmodeDueToMoveNextError    (PANICmode* pPanicMode, const COLstring& ErrorMessage);
void PANICmodeDueToLogValidateError (PANICmode* pPanicMode, const COLstring& ErrorMessage);
void PANICmodeDueToLogInitError     (PANICmode* pPanicMode, const COLstring& ErrorMessage);
void PANICmodeDueToInvalidLogLocker (PANICmode* pPanicMode, const COLstring& ErrorMessage);

#endif
