#ifndef __NTAB_CONSTANT_H__
#define __NTAB_CONSTANT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABconstant
//
// Description:
//
// NTABconstant - common constants.
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 10:34:13 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include <NOD/NODtype.h>

extern const COLstring NTABprotocolName;

enum NTABconstant{
   NTAB_TABLE_PROTOCOL=50,
   NTAB_ROOT=1,
   NTAB_TABLE=4,
   NTAB_TABLE_ROW=5
};

enum NTABtype{
   NTAB_STRING=6,
   NTAB_INTEGER=7, 
   NTAB_DOUBLE=8,
   NTAB_DATETIME=9
};

#endif // end of defensive include
