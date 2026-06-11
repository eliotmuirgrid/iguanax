#ifndef __WEB_CONNECTION_CHECK_H__
#define __WEB_CONNECTION_CHECK_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBconnectionCheck
//
// Description:
//
// Functions to set and delete a closure used to check whether a connection id
// is still in WEBserver's m_Connections map
//
// Author: Vismay Shah 
// Date:   Wednesday 17 May 2023 - 04:27PM
// ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLminimumInclude.h>

class WEBserver;

void WEBsetConnectionCheckClosure(COLclosure2<COLuint64, bool*>*& pClosure, WEBserver* pServer);
void WEBdeleteConnectionCheckClosure(COLclosure2<COLuint64, bool*>*& pClosure);

#endif // end of defensive include