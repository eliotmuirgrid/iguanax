// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITCutils.cpp
//
//  Description
//
// Author: Matthew Sobkowski
// Date: May 16, 2022
//---------------------------------------------------------------------------
#include <GITC/GITCutils.h>

#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// TODO - not sure I like that this function mixes three concerns together
WEBrequest GITCbitbucketRequest(const COLstring& Url, const COLstring& Username, const COLstring& AppPassword) {
   COL_FUNCTION(GITCbitbucketRequest);
   WEBrequest Request;
   Request.Url = Url;
   Request.UserName = Username;
   Request.Password = AppPassword;
   Request.Timeout = 5000;
   Request.Headers["Accept"] = "application/json";
   return Request;
}

void GITCextractErrorString(const WEBresponse& Response, const COLstring& ErrMsg, COLstring* pOut){
   COL_FUNCTION(GITCextractErrorString);
   COLstring HeaderValue;
   WEBheaderGetValue(Response.Headers, "Content-Type", &HeaderValue);
   COLstring Err = (HeaderValue == "text/html; charset=utf-8") ? Response.Body: ErrMsg;
   *pOut = Err;
}
