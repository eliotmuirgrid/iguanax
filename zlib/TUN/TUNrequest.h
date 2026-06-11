#ifndef __TUN_REQUEST_H__
#define __TUN_REQUEST_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNrequest
//
// Description:
//
// Request object - goal is to have more information to pass to event dispatching logic
//
// Author: Eliot Muir 
// Date:   Thursday 15 June 2023 - 02:27PM
// ---------------------------------------------------------------------------

#include <COL/COLstring.h>

class TUNrequest{
public:
   COLstring* pIn;
   COLstring* pOut;
   COLuint64 ConnectionId;
};

class TUNrequestItem{
public:
   COLstring RecordData;
   COLstring* pOut;
   COLuint64 ConnectionId;
};

#endif // end of defensive include
