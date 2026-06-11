#ifndef __DBDVER_VERSION_NUMBER_H__
#define __DBDVER_VERSION_NUMBER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDVERversionNumber
//
// Description:
//
// Web handler for serving up version JSON
//
// Author: Nicolas Lehman
// Date:   Mon Mar 15 2021
//---------------------------------------------------------------------------
class COLwebRequest;

void DBDVERgetVersionNumber (const COLwebRequest& Request);

#endif // end of defensive include
