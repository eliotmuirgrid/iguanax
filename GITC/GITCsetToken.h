#ifndef __GITC_SET_TOKEN_H__
#define __GITC_SET_TOKEN_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCsetToken
//
// Description:
//
// API for saving the access token for the cloud provider (or local host).
// For Bitbucket, this is the app password, for gitlab it is the access token.
//
// Author: Vismay Shah 
// Date:   Thursday 04 April 2024 - 01:00PM
// ---------------------------------------------------------------------------
class COLwebRequest;

void GITCsetToken(const COLwebRequest& Request);

#endif // end of defensive include
