#ifndef __WEB_REQUEST_DATA_H__
#define __WEB_REQUEST_DATA_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBrequestData
//
// Description:
//
// POD object we use to parse HTTP request data into .
//
// Author: Eliot Muir 
// Date:   Saturday 20 May 2023 - 02:57PM
// ---------------------------------------------------------------------------

#include <COL/COLstring.h>
#include <COL/COLmap.h>
#include <COL/COLweb.h>

class COLvar;
class WEBrequestData {
public:
   COLstring Path;
   COLmap<COLstring, COLstring> Args;  // GET arguments - on the URL itself
   COLmap<COLstring, COLstring> Headers;
   COLstring Body;
   COLuint64 ConnectionId;
   bool IsSecure;
};

// Assuming that COLvar for inputs are small the overhead won't matter - this is a more convenient
// function signature.
COLvar WEBrequestInputs(const WEBrequestData& Request);

COLostream& operator<<(COLostream& Stream, const WEBrequestData& Data);

#endif // end of defensive include
