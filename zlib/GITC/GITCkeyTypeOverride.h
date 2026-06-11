#ifndef __GITC_KEY_TYPE_OVERRIDE_H__
#define __GITC_KEY_TYPE_OVERRIDE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCkeyTypeOverride
//
// Description:
//
// Returns the value of the environment variable IFW_SSH_GEN_TYPE.
//
// Author: Vismay Shah 
// Date:   Monday 10 February 2025 - 05:56PM
// ---------------------------------------------------------------------------

class COLwebRequest;

void GITCkeyTypeOverride(const COLwebRequest& Request);

#endif // end of defensive include
