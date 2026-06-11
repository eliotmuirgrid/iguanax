#ifndef __LNET_SMTP_H__
#define __LNET_SMTP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETsmtp
//
// Description:
//
// LNETsmtp class.
//
// Author: Nasron Cheong
// Date:   Wednesday, March 9th, 2011 @ 02:21:40 PM
//
//---------------------------------------------------------------------------

struct lua_State;

void LNETaddSmtpModule(lua_State* L);

#endif // end of defensive include
