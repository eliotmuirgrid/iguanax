#ifndef __LUA_LINE_PARSER_H__
#define __LUA_LINE_PARSER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAlineParser
//
// Description:
//
// LUAlineParser - .see http://fogbugz.ifware.dynip.com/default.asp?W3630
//
// The idea of this code is something simple to parse a Lua text file and work
// out the length of each line.  Trailing space characters at the end of the
// line are not included in the line length.
//
// Author: Eliot Muir
// Date:   Wednesday, June 30th, 2010 @ 10:22:57 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

template <class Item> class COLarray;


void LUAparseLineLengths(const COLstring& LuaCode, COLarray<int>* pLineLengthVector);

int LUAmaxLineLengthInBlock(const COLarray<int>& pLineLengthVector, int Start, int End);

#endif // end of defensive include
