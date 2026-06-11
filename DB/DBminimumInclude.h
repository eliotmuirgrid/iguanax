#ifndef __DB_MINIMUM_INCLUDE_H__
#define __DB_MINIMUM_INCLUDE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBminimumInclude
//
// Description: 
//
// Minimal include file
//
// Author: Henry Tran
// Date:   January 9, 2004 
//
//
//---------------------------------------------------------------------------

// These are the supported data types of this library.

enum DBdataType
{
   DB_DATA_TYPE_NOT_DEFINED, // Denotes that a data type property needs to be defined.
   DB_NULL,                  // Note: NULL result set values are represented by a DB_DATA_TYPE_NOT_DEFINED //TODO fix behaviour
   DB_STRING,
   DB_TEXT,                  // For large strings
   DB_INTEGER,
   DB_DOUBLE,
   DB_DATETIME,
   DB_LARGE_INTEGER,
   DB_LARGE_DOUBLE,
   DB_BOOLEAN,
   DB_BINARY
};

#endif // end of defensive include
