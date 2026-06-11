// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USGusage
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Tue Oct  2 10:16:34 EDT 2024
// ---------------------------------------------------------------------------
#include <USG/USGusage.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


void USGusage::updateUploadTime() {
   time(&m_LastUpload);
   reset();
}

void USGusage::setLastError(const COLstring& Error) {
   m_LastError = Error;
}

// resets all usage data members except m_LastUpload
void USGusage::reset() {
   LoginCount = 0;
}

COLvar USGusage::toVar() const {
   COLvar Out;
   Out["since"] = COLintToString(m_LastUpload);
   Out["login_count"] = LoginCount;
   return Out;
}

USGusage& USGusages() {
   static USGusage s_Usages;
   return s_Usages;
}
