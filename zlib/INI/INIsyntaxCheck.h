#ifndef __INI_SYNTAX_CHECK_H__
#define __INI_SYNTAX_CHECK_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INIsyntaxCheck
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Thursday 24 August 2023 - 01:39PM
// ---------------------------------------------------------------------------

class INIdata;
class COLostream;
class COLstring;

bool INIsyntaxCheck(const INIdata& Expected, const INIdata& Data, COLstring* pOutput);

#endif // end of defensive include
