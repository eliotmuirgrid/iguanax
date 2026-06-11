#ifndef __PRM_COMPONENT_H__
#define __PRM_COMPONENT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PRMcomponent
//
// Description:
//
// Provided a component guid, this returns the permissions for the user
//
// Author: Tyler Brown 
// Date:   Monday 29 January 2024 - 07:53AM
// ---------------------------------------------------------------------------

struct CFGmap;
class COLwebRequest;

void PRMcomponent(const COLwebRequest& Request);

#endif // end of defensive include
