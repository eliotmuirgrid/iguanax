#ifndef __TBM_RESULT_H__
#define __TBM_RESULT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TBMresult
//
// Description:
//
// TBMresult enumeration of return codes from TBMparser
//
// Author: Eliot Muir
// Date:   Tuesday, February 13th, 2007 @ 02:00:51 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// Negative TBMresult codes are error conditions that indicate that the data is corrupt or unknown
// and that data transmission should stop.

enum TBMresult
{
   TBM_INTERNAL_ERROR=-2,
   TBM_UNKNOWN_BINARY_TOKEN=-1,    
   TBM_COMPLETE=0,
   TBM_INCOMPLETE=1,  // This means the data is not complete but we are not in the middle of a token
   TBM_INCOMPLETE_TOKEN=2, // This means the data is incomplete and we are in the middle of a token
   TBM_NEED_STRING_SIZE =3  // special case of TBM_NEED_MORE_DATA when we are just before the string size information
};

#endif // end of defensive include
