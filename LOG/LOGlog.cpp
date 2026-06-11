// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGlog
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 18 May 2023 - 02:40PM
// ---------------------------------------------------------------------------
#include <LOG/LOGlog.h>

#include <COL/COLauto.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLauto<COLclosure2<const LOGlogInfo&, const COLstring&> > s_pLogCallback;

void LOGlog(const LOGlogInfo& LogInfo, const COLstring& Data) {
   if(s_pLogCallback.get()) { s_pLogCallback->run(LogInfo, Data); }
}

// set the callback which probably uses SCKloop
void LOGlogSetCallback(COLclosure2<const LOGlogInfo&, const COLstring&>* pCallback) {
   COL_FUNCTION(LOGlogSetCallback);
   s_pLogCallback = pCallback;
}