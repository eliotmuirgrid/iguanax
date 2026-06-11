#ifndef __DBD_SERVER_HEADER_H__
#define __DBD_SERVER_HEADER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDserverHeader
//
// Description:
//
// TODO
//
// Author: Eliot Muir 
// Date:   Wednesday 21 September 2022 - 06:05PM
// ---------------------------------------------------------------------------

class COLwebRequest;
class IWEBconfig;

void DBDserverHeader(const COLwebRequest& Request, IWEBconfig* pConfig);

#endif // end of defensive include
