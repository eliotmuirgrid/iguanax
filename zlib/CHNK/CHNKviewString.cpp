// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CHNKviewString
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 07 December 2023 - 09:41AM
// ---------------------------------------------------------------------------

#include <CHNK/CHNKviewString.h>

#include <SFI/SFIhtmlEscape.h>
#include <THTM/THTMprinter.h>
#include <DIS/DISbinaryDisplay.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

CHNKviewMode CHNKviewModeArg(const COLstring& ViewModeArg){
   COL_FUNCTION(CHNKviewModeArg);
   CHNKviewMode ViewMode = CHNK_VIEW_PLAIN_TEXT;
   if(ViewModeArg == "string_et") ViewMode = CHNK_VIEW_ESCAPED_TEXT;
   if(ViewModeArg == "string_hx") ViewMode = CHNK_VIEW_HEX;
   return ViewMode;
}

void CHNKrenderViewString(const COLstring& OriginalString, const CHNKviewMode& ViewMode, COLstring* pViewString){
   COL_FUNCTION(CHNKrenderViewString);
   COLstring& ViewString = *pViewString;
   ViewString.clear();
   switch (ViewMode){
      case CHNK_VIEW_PLAIN_TEXT  : SFIhtmlEscapeNoNewline(OriginalString.c_str(), OriginalString.size(), ViewString);                                                  break;
      case CHNK_VIEW_ESCAPED_TEXT: {COLostream OutStream(ViewString); THTMprintEscapedLiteralWithNewlines(OutStream, OriginalString.c_str(), OriginalString.size()); } break;
      case CHNK_VIEW_HEX         : {COLostream OutStream(ViewString); DISbinaryDisplay::displayData      (OutStream, OriginalString.c_str(), OriginalString.size()); } break;
      default: COLPRECONDITION(false);
   }
}
