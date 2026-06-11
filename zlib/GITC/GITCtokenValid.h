#ifndef __GITC_TOKEN_VALID_H__
#define __GITC_TOKEN_VALID_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCtokenValid
//
// Description:
//
// Api to check if the token is set and is valid
//
// Author: Vismay Shah 
// Date:   Thursday 04 April 2024 - 02:45PM
// ---------------------------------------------------------------------------
class COLwebRequest;
class COLstring;

void GITCtokenValid(const COLwebRequest& Request);

#endif // end of defensive include
