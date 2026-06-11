#ifndef __PRO_WEB_H__
#define __PRO_WEB_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROweb
//
// Description:
//
// Opens URL - we put this into the PRO library since we need to invoke
// a process and under Linux gio open spits out a lot of garbage.
//
// Author: Eliot Muir 
// Date:   Friday 14 July 2023 - 11:57AM
// ---------------------------------------------------------------------------

class COLstring;

bool PROweb(const COLstring& Url);

#endif // end of defensive include
