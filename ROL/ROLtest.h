#ifndef __ROLTEST_H__
#define __ROLTEST_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLtest.h
//
//  Description: Safety check to test whether new role will lock user out of editing roles
//
//  Author: Matthew Sobkowski
//  Date:   28/02/24 1:50 PM
//  ---------------------------------------------------------------------------
#include <COL/COLweb.h>

class ROLrole;
class ROLmap;

bool ROLtestImpl(const COLarray<COLstring>& Roles, ROLmap* pRoleMap);
void ROLtest(const COLwebRequest& Request, ROLmap* pRoleMap);

#endif // end of defensive include
