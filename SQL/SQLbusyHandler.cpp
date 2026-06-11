//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SQLbusyHandler
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Thursday, December 14th, 2006 @ 02:41:36 PM
//
//---------------------------------------------------------------------------
#include "SQLprecomp.h"
#pragma hdrstop

#include <SQL/SQLbusyHandler.h>

SQLbusyHandler::SQLbusyHandler() : Interval(10){}

COLuint32 SQLbusyHandler::sqliteBusyInterval() const{
   return Interval;
}

void SQLbusyHandler::setSqliteBusyInterval(COLuint32 iInterval){
   Interval = iInterval;
}
