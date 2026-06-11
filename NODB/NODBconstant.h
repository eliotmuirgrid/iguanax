#ifndef __NODB_CONSTANT_H__
#define __NODB_CONSTANT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODBconstant
//
// Description:
//
// Defines constants for NODB.
//
// Author: Kevin Senn
// Date:   Thursday, June 17th, 2010 @ 03:17:17 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include <NOD/NODtype.h>

extern const COLstring NODBprotocolName;

enum NODBconstant
{
   NODB_DB=20,
   NODB_RESULT_SET=1,
   NODB_ROW=2,
   NODB_VALUE_STRING=3,
   NODB_VALUE_INTEGER=4,
   NODB_VALUE_DOUBLE=5,
   NODB_VALUE_DATETIME=6,
   NODB_VALUE_BINARY=7
};

#endif // end of defensive include
