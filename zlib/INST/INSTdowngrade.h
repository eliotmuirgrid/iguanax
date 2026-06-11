#ifndef __INSTDOWNGRADE_H__
#define __INSTDOWNGRADE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTdowngrade
//
// Description: Downgrade to an older version
//
// Author: Matthew Sobkowski
// Date:   Friday 25 August 2023 - 10:36PM
// ---------------------------------------------------------------------------
class COLstring;

class COLwebRequest;

void INSTdowngradeList(const COLwebRequest& Request);


#endif //end of defensive include
