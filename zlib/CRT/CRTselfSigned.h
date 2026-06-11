#ifndef __CRT_SELF_SIGNED_H__
#define __CRT_SELF_SIGNED_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CRTselfSigned
//
// Description:
//
// API for generating a self signed certificate
//
// Author: John Q
// Date:   Sun 20 Aug 2023 12:08:03 PDT
// ---------------------------------------------------------------------------

class COLstring;

bool CRTgenerateSelfSignedCertKey(COLstring* pError);

#endif // end of defensive include
