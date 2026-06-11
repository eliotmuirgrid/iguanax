// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKBhostResolve
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 03 October 2023 - 02:14PM
// ---------------------------------------------------------------------------

#include <SCKB/SCKBhostResolve.h>

#include <SCK/SCKsystem.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool SCKBhostResolve(const COLstring& Host, int Port, struct addrinfo*& Address) {
   COL_FUNCTION(SCKBhostResolve);
   struct addrinfo Hints;
   memset(&Hints, 0, sizeof(Hints));
   // For some reason IPv6 addresses don't appear to work with this library.  So we are just querying AF_NET for IPv4 addresses
   // See IX-2273 for more detail.
   Hints.ai_family   = AF_INET;     // AF_INET == Just IPV4 addresses
   Hints.ai_socktype = SOCK_STREAM; // TCP
   return 0 == ::getaddrinfo(Host.c_str(), COLintToString(Port).c_str(), &Hints, &Address);
}