#ifndef __NOD_TYPE_H__
#define __NOD_TYPE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODtype
//
// Description:
//
// Contains macro definitions to define and "register" protocol and node
// types, so that NODprotocolNameToId() and NODnodeTypeNameToId() will
// correctly map protocol/node-type names to ids.
//
// Author: Kevin Senn
// Date:   Friday, December 23rd, 2011 @ 04:39:26 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// Output is the protocol and node type.  These may be -1 if either one is not found.
// CombinedName should be in the format: "<protocol name>:<node type name>".
void NODprotocolAndNodeTypeNameToIds(const COLstring& CombinedName, int& Protocol, int& NodeType);

// void NODtypeMapInit();
void NODtypeMapAdd(const COLstring& NodeTypeName, int NodeType, const COLstring& ProtocolName, int Protocol);

#endif // end of defensive include
