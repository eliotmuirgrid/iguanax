#ifndef __WEB_API_H__
#define __WEB_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBapi
//
// Description:
//
// We can register some of the login etc. functions here.
//
// Author: Eliot Muir
// Date:   Thursday 25 May 2023 - 09:30PM
// ---------------------------------------------------------------------------

#include <COL/COLweb.h>
#include <COL/COLhashmap.h>
#include <COL/COLmap.h>

class WEBsession;
class USRuser;

void WEBapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, COLmap<COLstring, WEBsession>* pSessionMap, 
   COLmap<COLstring, USRuser>* pUserMap, COLclosure3<COLstring, COLstring, COLclosure1<COLvar>*>* pExternalAuthenticator);

#endif // end of defensive include
