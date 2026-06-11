#ifndef __PRT_UTILS_H__
#define __PRT_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PRTutils
//
// Description:
//
// General purpose port utilities.
//
// Author: Cheryl Chan
// Date:   Monday, March 12th, 2007 @ 10:36:34 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>

enum PRTreturn
{
   PRT_PORT_FREE    = 0,
   PRT_PORT_USED    = 1,
   PRT_SOCKET_ERROR = 2,
   PRT_INVALID_PORT = 3,
   PRT_PORT_DEFINED_IN_ENV = 4
};

PRTreturn PRTisPortInUse(COLuint16 Port, const COLstring& Interface="");
PRTreturn PRTmainCheckPort(COLostream& LogStream, COLuint16 Port);

#endif // end of defensive include
