#ifndef __LHLP_HELP_H__
#define __LHLP_HELP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHLPhelp
//
// Description:
//
// LHLPhelp class.
//
// Author: Nasron Cheong
// Date:   Wednesday, September 21st, 2011 @ 10:36:33 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLarray.h>

COLstring LHLPhelpTruncateText(const COLstring& Text, COLuint32 MaxLength);

void LHLPhelpAddStyledValue(COLostream& Stream, const COLstring& Val);

#endif // end of defensive include
