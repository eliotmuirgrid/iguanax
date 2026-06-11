// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNapiKeyPublic
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 29 June 2023 - 01:33PM
// ---------------------------------------------------------------------------

#include <TUN/TUNapiKeyPublic.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNkeyConvert.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /tunnel/key/public
void TUNapiKeyPublic(const COLwebRequest& Request, TUNstateClient* pState){
   COL_FUNCTION(TUNapiKeyPublic);
   COLvar Result;
   Result["key"] = TUNkeyPublicAsPemString(pState->m_MyKey);
   COLrespondSuccess(Request.Address, Result);
}