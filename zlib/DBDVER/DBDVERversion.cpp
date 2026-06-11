//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDVERversion
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sat 19 Sep 2020 10:31:47 EDT
//---------------------------------------------------------------------------

#include <DBDVER/DBDVERversion.h>
#include <COL/COLstring.h>

#ifdef DBD_SOURCE_TAG_OVERRIDE
   const char* DBD_SOURCE_TAG = DBD_STRINGIFY(DBD_SOURCE_TAG_OVERRIDE) ;
   const char* DBD_BUILD_TYPE_SNAPSHOT = "snapshot-" DBD_STRINGIFY(DBD_SOURCE_TAG_OVERRIDE) ;
#else
   const char* DBD_SOURCE_TAG = /*{SOURCE_TAG*/ 0 /*}*/ ;
   const char* DBD_BUILD_TYPE_SNAPSHOT = "snapshot";
#endif


