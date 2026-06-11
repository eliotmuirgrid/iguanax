#ifndef __LNET_HTTP_LISTEN_H__
#define __LNET_HTTP_LISTEN_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNEThttpListen
//
// Description:
//
// net.http.listen api for Lua. Sets up a SCKlistener to listen on a port, and
// send complete http requests to the main function of the Lua script.
//
// Author: Vismay Shah
// Date:   Friday 27 October 2023 - 10:08AM
// ---------------------------------------------------------------------------
#include <COL/COLstring.h>
#include <openssl/ssl.h>

struct lua_State;
class WEBrequestData;
class BLUAinstance2;

int LNEThttpListen(lua_State* L);


// internal
int  LNETsslAlpnSelector(SSL *ssl, const unsigned char **out, unsigned char *outlen, const unsigned char *in, unsigned int inlen, void *arg);
bool LNETisHttp2Connection(BLUAinstance2* pInstance, int ConnId);
bool LNETfoundHttp2Preface(const COLstring& Buffer);
bool LNETisHttp2UpgradeRequest(const WEBrequestData& Request);
bool LNETisHttp2Connection(BLUAinstance2* pInstance, int ConnId);
void LNETsetupHttp2Connection(BLUAinstance2* pInstance, int ConnId, const COLstring& UpgradeRequest = "", const COLstring& Http2Setting = "");
void LNETteardownHttp2Connection(BLUAinstance2* pInstance, int ConnId);
void LNEThandleHttp2Data(COLstring* pData, BLUAinstance2* pInstance, int ConnId);
void LNEThttp2OnWrite(BLUAinstance2* pInstance, int ConnId);

#endif // end of defensive include
