#ifndef __CMD_EXAMPLE_PRECOMP_H__
#define __CMD_EXAMPLE_PRECOMP_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Standard precompiled header include file
// 
// Author: Roman Leifer
// Date:   Tue 08/17/2004 
//
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <time.h>

#include <COL/COLerror.h>
#include <COL/COLostream.h>
#include <COL/COLmath.h>
#include <COL/COLarray.h>
#include <COL/COLdateTime.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestable.h>
#include <TST/TSTtestFixture.h>
#include <TST/TSTtestMethod.h>
#include <TST/TSTtestFunction.h>
#include <TST/TSTconsoleApplication.h>

#define CMD_LINE_PARSER_FULL
#include <CMD/CMDlineParser.h>

#endif // end of defensive include
