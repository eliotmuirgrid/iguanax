#ifndef __LUAL_PRINT_H__
#define __LUAL_PRINT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUALprint
//
// Description:
//
// Contains methods used for the serialization of Lua items (with our own
// special formatting using metatables).
//
// Author: Kevin Senn
// Date:   Friday, December 23rd, 2011 @ 11:44:22 AM
//
//---------------------------------------------------------------------------
#include <COL/COLlist.h>
#include <COL/COLminimumInclude.h>

#include <COL/COLarray.h>
#include <COL/COLminimumInclude.h>

#include <LUA/LUAheader.h>


class LUALprintCallback;

// If the table is in the stack, returns true and prints to Stream a string representing the
// the table's position in the stack.  e.g., top is "<self>", second from the top is "<parent up 1 level>", etc.
// pCallback can be NULL, but if it is non-null it will be used to nicely format the string
// printed to Stream.
//bool LUALisTableInStack(const void* pTable, const COLarray<const void*>& TableStack, COLostream& Stream, bool LUALprint);

// Gets display information for a userdata or table item.
// The four outputs are determined through the _display metamethod.
// If _display is not defined, ProtocolType will be DefaultProtocolType,
// and NodeType will the Lua type identifier.
// If _display is not defined, The ShortLabel and LongLabel will be:
//  1) Both the result of __tostring(),
//  2) Both the metatable name, or
//  3) The Lua typename (i.e. "userdata" or "table"), and the
//     serialized table (if it's table; "userdata" otherwise).
// Returns true if anything other than standard (easily reproducible from
// a copied tree) Lua serialization was used.  This includes:
//  a) _display results,
//  b) __tostring results,
//  c) metatable names, and
//  d) serialized Lua tables, as they may contain table loops (which
//     would be lost in a copied tree).
// pCallback can be NULL, but if it is non-null it will be used to nicely format the string
// printed to Stream.
//
bool LUALdisplayComplexItem(
   lua_State* L,
   int i,
   COLstring* pShortLabel, // output
   COLstring* pLongLabel, // output, if NULL it will be ignored.
   int& ProtocolType, // output
   int& NodeType, // output
   COLarray<const void*>* pTableStack, // can be NULL; an empty TableStack will be used.
   bool LUALprint
);

// Prints a Lua item to the stream.  If the item is complex, will return
// the "long label" from LUALdisplayComplexItem() (or the "short label"
// if the "long label" is empty).
void LUALprintItem(lua_State* L, int i, COLostream& Stream, bool LUALprint = false);

bool LUALhtmlFilterTableItem(lua_State* L, int TableIndex, int KeyIndex, int ValueIndex);

#endif // end of defensive include
