#ifndef __INST_ADMIN_H__
#define __INST_ADMIN_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTworkingDir
//
// Description:
//
// IguanaX admin user exists and create API
// Note: These APIs depends on the INSTworkingDir API to correctly set the
// currently working dir so that they could check and set the admin user in
// the correct IguanaX working dir.
//
// Author: Tyler Brown 
// Date:   Tuesday 08 August 2023 - 01:17PM
// ---------------------------------------------------------------------------

class COLwebRequest;

void INSTensureAdmin(const COLwebRequest& Request);
void INSTadminCreate(const COLwebRequest& Request);

#endif // end of defensive include
