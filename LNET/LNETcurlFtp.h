#ifndef __LNET_CURL_FTP_H__
#define __LNET_CURL_FTP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETcurlFtp
//
// Description:
//
// LNETcurlFtp class.
//
// Author: Nasron Cheong
// Date:   Monday, March 28th, 2011 @ 03:12:38 PM
//
//---------------------------------------------------------------------------

struct lua_State;

extern const char* LNET_FTP_META_TABLE;
extern const char* LNET_SSH_FTP_META_TABLE;
extern const char* LNET_FTP_SSL_META_TABLE;

//assumes that the base table to add to is at the
//top of stack
void LNETaddFtpModule(lua_State* L);

void LNETcreateCurlFtpCache(lua_State* L);
void LNETcurlFtpInactiveCleanup(lua_State* L);

#endif // end of defensive include
