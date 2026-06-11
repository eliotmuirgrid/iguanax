#ifndef __NTAB_TABLE_ROW_G_H__
#define __NTAB_TABLE_ROW_G_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABtableRowG
//
// Description:
//
// NTABtableRowG - represents the grammar of a row.  Mostly it just points to NTABtable.
// But it does have some cached information
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 12:18:25 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class NTABtableRowG 
{
public:
   NTABtableRowG();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~NTABtableRowG();

private:
   NTABtableRowG(const NTABtableRowG& Orig); // not allowed
   NTABtableRowG& operator=(const NTABtableRowG& Orig); // not allowed
};

#endif // end of defensive include
