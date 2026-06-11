// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DEPmessaging
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 16 April 2025 - 09:23AM
// ---------------------------------------------------------------------------

#include <DEP/DEPmessaging.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring DEPmessage(const COLstring& FunctionName){
   COL_FUNCTION(DEPmessage);
   COLstring Message = FunctionName.is_null() ? "a deprecated Iguana API." : "the deprecated Iguana API '" + FunctionName + "'";
   return "The script being run contains " + Message;
}

COLstring DEPtooltipHtml(const COLstring& Component){
   COL_FUNCTION(DEPtooltipHtml);
   COLstring Message = DEPmessage("");
   COLstring Html = Message + " <a href='#logs?component=" + Component + "&search=deprecated'>View Logs</a>";
   return "<div class='DEPhtml'>" + Html + "</div>";
}