#ifndef MAIN2PROJECT_HATYPES_H
#define MAIN2PROJECT_HATYPES_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HAtypes.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLstring.h>

enum HAtype {
   NONE,
   ACTIVE_PASSIVE_SHARED_DRIVE,
};

HAtype HAstringToType(const COLstring& Type);
COLstring HAtypeToString(const HAtype& Type);


#endif  // end of defensive include
