#ifndef __SDM_HANDLE_H__
#define __SDM_HANDLE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMhandle
//
// Description:
//
// Narrowly scoped RAII helper to clean up the Windows service HANDLEs.
//
// Example:
//
// {
//    SDMhandle Service;
//    Service.handle = ::CreateService(...);
// }  // auto close Service.handle
//
// Author: John Q
// Date:   Mon 28 Aug 2023 09:02:30 PDT
// ---------------------------------------------------------------------------

#include <COL/COLwindows.h>

class SDMhandle {
public:
   SDMhandle(){}
   ~SDMhandle();
   SC_HANDLE handle = nullptr;
};

#endif // end of defensive include
