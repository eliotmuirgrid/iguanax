#ifndef __RGN_HASH_H__
#define __RGN_HASH_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: RGNhash
//
// Description:
//
// RGNhash class.
//
// Author: Art Harrison
// Date:   Thursday, July 1st, 2010 @ 06:51:34 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

COLstring RGNhash5(const COLstring& Padding, const COLstring& Value);

COLstring RGNmachineIdFromHostName(const COLstring& HostName);

#endif // end of defensive include
