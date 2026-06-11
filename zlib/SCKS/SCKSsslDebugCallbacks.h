#ifndef __SCKS_SSL_DEBUG_CALLBACKS_H__
#define __SCKS_SSL_DEBUG_CALLBACKS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKSsslDebugCallbacks
//
// Description:
//
// Callbacks which give status of SSL handshake etc.  Quite chatty.
// This is called from the prepare method of SCKSsslConnection.
//
// Author: Eliot Muir 
// Date:   Wednesday 21 June 2023 - 10:44AM
// ---------------------------------------------------------------------------

struct ssl_st;
typedef struct ssl_st SSL;

void SCKSsslDebugRegister(SSL* pSSL, bool IsServer);
void SCKSreportFatalError(int ErrorCode, int RC);

#endif // end of defensive include