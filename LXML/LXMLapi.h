#ifndef __LXML_API_H__
#define __LXML_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LXMLapi
//
// Description:
//
// LXMLapi class.
//
// Author: Kevin Senn
// Date:   Wednesday, March 9th, 2011 @ 10:12:07 AM
//
//---------------------------------------------------------------------------
struct lua_State;


int LXMLparseXml(lua_State* L);

void LXMLaddXmlNamespace(lua_State* L);

#endif // end of defensive include
