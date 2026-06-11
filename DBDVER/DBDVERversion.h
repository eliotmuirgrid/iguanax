#ifndef __DBDVER_VERSION_H__
#define __DBDVER_VERSION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDVERversion
//
// Description:
//
// Application version file.
//
// Author: Nasron Cheong
// Date:   May, 2004
//---------------------------------------------------------------------------

#define DBD_APP_NAME "Iguana"
#define DBD_VERSION_MAJOR 10 
#define DBD_VERSION_MINOR 1

#define DBD_BUILD_NUMBER 119

// macro magic to inline constants
#define DBD_ASSTR(x) #x
#define DBD_STRINGIFY(x) DBD_ASSTR(x)

#define DBD_STRING_VERSION DBD_STRINGIFY(DBD_VERSION_MAJOR) "." DBD_STRINGIFY(DBD_VERSION_MINOR) "." DBD_STRINGIFY(DBD_BUILD_NUMBER)
 
#endif // end of defensive include
