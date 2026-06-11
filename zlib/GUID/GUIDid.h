#ifndef __GUID_ID_H__
#define __GUID_ID_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2017 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GUIDid
//
// Description:
//
// Utilities for generating GUIDs.
//
// Author:  Akshay Ganeshen
// Date:    Wed Mar 25 10:13:36 EDT 2015
//---------------------------------------------------------------------------

#include <COL/COLstring.h>

// Generates alphanumeric-encoded GUIDs (lowercase, uppercase, and numbers)
// with the same amount of total possibilities as a normal GUID (128-bits)
// The length of these GUIDs are 22 characters
COLstring GUIDnewId();

// Generates a shorter, 14-character GUID, which is more human-readable
// With some analysis, this still guarantees that:
//    After generating 1 million GUIDs a day for 25 years...
//    the probability of a collision is about 0.000335%
COLstring GUIDnewShortId();

// Generates a very short, 8-character GUID, which is easily human-readable
// Not much collision resistance, so use it infrequently
COLstring GUIDnewTinyId();

// Custom-length GUID generation - the above methods just call this one
COLstring GUIDnewId(int Length);

#endif // end of defensive include
