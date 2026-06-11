//------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  SCKtimer
// 
// Description: Implementation 
//
// Author:   Eliot Muir
// Date:     Fri  4 Sep 2020 10:16:44 EDT
//---------------------------------------------------------------------------

#include "SCKtimer.h"
#include "SCKloop.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

SCKtimer::SCKtimer(SCKloop* pLoop) : m_pLoop(pLoop), m_TimerId(-1){
   COL_METHOD(SCKtimer::SCKtimer);
}

SCKtimer::~SCKtimer() {
   COL_METHOD(SCKtimer::~SCKtimer);
   stop();
}

void SCKtimer::stop() {
   COL_METHOD(SCKtimer::stop);
   if (m_pLoop->isSet(m_TimerId)){
      COL_TRC("Cancelling time.");
      m_pLoop->cancelTimer(m_TimerId);
      m_TimerId = -1;
   }
}

bool SCKtimer::isSet() const{
   COL_METHOD(SCKtimer::isSet);
   return m_pLoop->isSet(m_TimerId);
}

void SCKtimer::start(COLauto<COLclosure0> pCallback, int IntervalInMilliseconds, bool Repeat){
   COL_METHOD(SCKtimer::start);
   stop();
   m_TimerId = m_pLoop->addTimer(pCallback, IntervalInMilliseconds, Repeat);
   COL_VAR4(pCallback.get(), IntervalInMilliseconds, Repeat, m_TimerId);
}
