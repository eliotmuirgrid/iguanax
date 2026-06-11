#ifndef __LND_UTILS_H__
#define __LND_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNDutils
//
// Description:
//
// Lua bindings for NOD
//
// Author: Eliot Muir
// Date:   Tuesday, May 18th, 2010 @ 12:47:45 PM
//
//---------------------------------------------------------------------------

class NODeventListener;
class NODplace;
class TNDLstackInfo;
class NODaddress;
class TNDLplaceAddress;
class TRNannotationState;

struct lua_State;

#include <COL/COLref.h>
#include <NOD/NODaddress.h>

bool LNDisNodeWithAddress(lua_State *L, int i);

// Returns true if the node is a node (with or without an address).
// If you're not sure whether a userdata item is a node or not, call this
// before calling LNDgetNode(), which will throw an error if the item is
// not a node.
bool LNDisNode(lua_State* L, int i);

// Gets the NODplace from the specified stack index (if the item is a NODplace
// or a NODplaceAddress - returns NULL otherwise).
NODplace* LNDgetNode(lua_State* L, int i);

// Gets the NODaddress from the specified stack index.
const NODaddress& LNDgetNodeAddress(lua_State* L, int i);

// TODO - odd to define an important API here?
void LNDdefineTree(lua_State* L);

void LNDpushNode(lua_State* L, NODplace* pNode);
void LNDpushNodeWithAddress(lua_State* L, NODplace* pNode, const NODaddress& Address);


extern const char* LND_TREE_METATABLE;
extern const char* LND_TREE_METATABLE_WITH_ADDRESS;

class LNDplaceAddress{
public:
   LNDplaceAddress()  {}
   ~LNDplaceAddress() {}
   
   COLref<NODplace> m_pPlace;
   NODaddress       m_Address;
};

#endif // end of defensive include
