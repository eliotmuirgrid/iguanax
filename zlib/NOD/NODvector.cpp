//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODvector
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, September 21st, 2010 @ 04:32:42 PM
//
//---------------------------------------------------------------------------
#include "NODprecomp.h"
#pragma hdrstop

#include "NODvector.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

NODvector::NODvector()
{
}

NODvector::~NODvector()
{
}

bool NODvector::isNull() const{
   return !size();
}
