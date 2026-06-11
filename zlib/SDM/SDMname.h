#ifndef __SDM_NAME_H__
#define __SDM_NAME_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMname
//
// Description:
//
// Service name for IguanaX.
//
// Author: Eliot Muir
// Date:   Thursday 22 September 2022 - 03:44PM
// ---------------------------------------------------------------------------

class COLstring;

void SDMsetServiceName(const COLstring& Name);

COLstring SDMserviceName();

#endif // end of defensive include
