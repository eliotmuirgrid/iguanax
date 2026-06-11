//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODsimple
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, September 21st, 2010 @ 04:16:45 PM
//
//---------------------------------------------------------------------------
#include "NODprecomp.h"
#pragma hdrstop

#include "NODsimple.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

NODsimple::NODsimple()
{
}

NODsimple::~NODsimple()
{
}

void NODsimple::clear(){
   setValue(NODplace::NOD_EMPTY_SIMPLE_VALUE);
}
