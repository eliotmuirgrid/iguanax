#ifndef __GITSEXISTS_H__
#define __GITSEXISTS_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITSexists.h
//
//  Description: Safe mode api that checks for git on the path
//
//  Author: Matthew Sobkowski
//  Date:   5/2/23 3:12 PM
//  ---------------------------------------------------------------------------
class COLwebRequest;

void GITSexists(const COLwebRequest& Request);

#endif // end of defensive include
