#ifndef __GINS_INITIALIZE_H__
#define __GINS_INITIALIZE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSinitialize
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Tuesday 10 September 2024 - 08:59AM
// ---------------------------------------------------------------------------

class COLstring;

void GINSinitialize();
void GINSaddComponent(const COLstring& CompId);
void GINSaddComponentList(const COLarray<COLstring>& CompList);
void GINSremoveComponent(const COLstring& CompId);
void GINSremoveComponentList(const COLarray<COLstring>& CompList);

#endif // end of defensive include
