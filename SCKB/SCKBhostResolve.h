#ifndef __SCKB_HOST_RESOLVE_H__
#define __SCKB_HOST_RESOLVE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKBhostResolve
//
// Description:
//
// Routine to resolve host address
//
// Author: Eliot Muir 
// Date:   Tuesday 03 October 2023 - 02:14PM
// ---------------------------------------------------------------------------

struct addrinfo;
class COLstring;

bool SCKBhostResolve(const COLstring& Host, int Port, struct addrinfo*& Address);

#endif // end of defensive include
