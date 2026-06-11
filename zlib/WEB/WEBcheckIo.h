#ifndef __WEB_CHECK_IO_H__
#define __WEB_CHECK_IO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBcheckIo
//
// Description:
//
// Routines which switch on asserts for checking for IO on the main thread.
//
// Author: Eliot Muir 
// Date:   Thursday 12 January 2023 - 11:39AM
// ---------------------------------------------------------------------------

class COLvar;

// This one is for throwing general calls.
void WEBcheckCall(const COLvar& Input);

#endif // end of defensive include
