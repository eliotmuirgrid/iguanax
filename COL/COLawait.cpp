//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLawait
//
// Description:
//
// Implementation.
//
// Author: John Q
// Date:   Tue 11 Oct 2022 9:32:55 EDT
//---------------------------------------------------------------------------
#include "COLawait.h"

#include <COL/COLerror.h>
#include <COL/COLlocker.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void COLawait::wait() {
   COL_METHOD(COLawait::wait);
   m_WaitThread = COLthreadGetCurrentThreadID();
   COLlocker Lock(m_Mutex);
   COLASSERT(m_Waiting == false);
   if (m_Synced) {
      COL_TRC("Already synced. Done");
      m_Synced = false;
      return;
   }
   COL_TRC("Waiting for sync...");
   m_Waiting = true;
   while (m_Waiting) {
      m_CV.wait(&m_Mutex);
   }
};

void COLawait::sync() {
   COL_METHOD(COLawait::sync);
   COLASSERT(m_WaitThread != COLthreadGetCurrentThreadID());
   COL_TRC("Notifying wait...");
   COLlocker Lock(m_Mutex);
   m_Synced  = true;
   m_Waiting = false;
   m_CV.broadcast();
};
