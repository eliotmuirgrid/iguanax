#ifndef __GIT_CHECKOUT_H__
#define __GIT_CHECKOUT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITcheckout
//
// Description:
//
// Checkout routines
//
// Author: Eliot Muir 
// Date:   Tuesday 14 February 2023 - 01:33PM
// ---------------------------------------------------------------------------

class COLstring;

bool GITcheckout(const COLstring& Dir, const COLstring& CommitId, COLstring* pError);

#endif // end of defensive include
