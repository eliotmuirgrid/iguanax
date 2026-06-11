#ifndef __CHNK_VIEW_STRING_H__
#define __CHNK_VIEW_STRING_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CHNKviewString
//
// Description:
//
// A library for chunking and returning sections of large strings.
//
// Author: Tyler Brown 
// Date:   Thursday 07 December 2023 - 09:41AM
// ---------------------------------------------------------------------------

class COLstring;

enum CHNKviewMode{
   CHNK_VIEW_PLAIN_TEXT,
   CHNK_VIEW_ESCAPED_TEXT,
   CHNK_VIEW_HEX
};

CHNKviewMode CHNKviewModeArg(const COLstring& ViewModeArg);

void CHNKrenderViewString(const COLstring& OriginalString, const CHNKviewMode& ViewMode, COLstring* pViewString);

#endif // end of defensive include
