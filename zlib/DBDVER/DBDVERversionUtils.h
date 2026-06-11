#ifndef __DBDVER_VERSION_UTILS_H__
#define __DBDVER_VERSION_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDVERversionString
//
// Description:
//
// String value of the current version number of Iguana
//
// Author: Vismay Shah
//---------------------------------------------------------------------------

class COLostream;
class COLstring;

#define DBDVER_BUILD_ID_NONE "N/A"

COLstring DBDVERstartupVersionInfo();
COLstring DBDVERbuildTypeString();
COLstring DBDVERversionString();
COLstring DBDVERbuildId(const char* NoneValue=DBDVER_BUILD_ID_NONE);
COLstring DBDVERstartupMessageWithVersionInfo(const COLstring& ConfigurationDirectory);
bool DBDVERversionOk();

COLstring DBDVERversionTag();

#endif // end of defensive include
