#ifndef __GITC_SET_SSH_H__
#define __GITC_SET_SSH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCsetSsh
//
// Description:
//
// Api to register ssh keys to the git host
//
// Author: Vismay Shah 
// Date:   Friday 05 April 2024 - 10:59AM
// ---------------------------------------------------------------------------
class COLwebRequest;

void GITCsetSsh(const COLwebRequest& Request);

#endif // end of defensive include
