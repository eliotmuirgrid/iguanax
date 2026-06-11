#ifndef __GITC_ALL_URLS_H__
#define __GITC_ALL_URLS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCallUrls
//
// Description:
//
// Takes a git url in, parses it and returns all forms of the git url back
// or an error if it fails to parse.
//
// Author: Vismay Shah 
// Date:   Tuesday 25 June 2024 - 02:12PM
// ---------------------------------------------------------------------------
class COLwebRequest;

void GITCallUrls(const COLwebRequest& Request);

#endif // end of defensive include
