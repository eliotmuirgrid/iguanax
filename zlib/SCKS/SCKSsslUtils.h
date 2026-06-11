#ifndef __SCKS_SSL_UTILS_H__
#define __SCKS_SSL_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKSsslUtils
//
// Author: Kevin Cai
//
// Description:
//
// Helper functions for setting up / cleaning SSL library and error messages.
//---------------------------------------------------------------------------

#include <COL/COLstring.h>

class SCKSsslContext;

// Returns all the errors in the SSL library Error queue and clears the queue
COLstring SCKSsslError();

// Helper function to set up a server ssl context
// 0 = Success, -1 = failure
int SCKSsetServerCtx(SCKSsslContext& Ctx, const COLstring& Certificate, const COLstring& PrivateKey);

// Sets up a client ctx that does not verify certificates.
// TODO - this should change to a pointer to be consistent with our conventions.
int SCKSsetClientCtx(SCKSsslContext& Ctx);

// SSL library initialization/destruction
void SCKSinitSSL();
void SCKSfreeSSL();

class SCKSsslInit {
public:
   SCKSsslInit();
   ~SCKSsslInit();
};
#endif // End of defensive guard
