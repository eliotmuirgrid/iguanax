#ifndef __WEB_RESPONSE_H__
#define __WEB_RESPONSE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBresponse
//
// Description:
//
// WEB response object - moved from WEBrequest.  POD structure that represents a response.
//
// Author: Eliot Muir 
// Date:   Sunday 21 May 2023 - 10:32AM
// ---------------------------------------------------------------------------

#include <COL/COLmap.h>
#include <COL/COLstring.h>

class WEBserver;

class WEBresponse{
public:
   int ReturnCode=503;
   COLmap<COLstring, COLstring> Headers;
   COLstring Body;
};

bool WEBheaderGetValue(const COLmap<COLstring, COLstring>& Headers, const COLstring& Key, COLstring* pValue);

void WEBheadersSetContentLength(int Size, COLstring* pOut);
void WEBheaderSetContentType(const COLstring& MimeType, COLstring* pOut);

// Just headers for a body result
void WEBresponseBodyHeaders(int Size, COLstring* pResult);

void WEBresponseJson(const COLvar& Data, COLstring* pResult);

void WEBresponseError(int ConnectionId, const COLstring& Error);
void WEBresponseErrorGen(const COLstring& Error, COLstring* pResult);

void WEBresponseHttpError(int BadErrorCode, const COLstring& ErrorMessage, COLstring* pResult);
void WEBresponseFile(const COLstring& MimeType, const COLstring& FileBody, COLstring* pResult);
void WEBresponseFileGzip(const COLstring& MimeType, const COLstring& FileBody, COLstring* pResult);



COLostream& operator<<(COLostream& Stream, const WEBresponse& Response);


void WEBresponseSetServer(WEBserver* pServer);

void WEBresponseSend(COLuint64 ConnectionId, const COLstring& Data);         // This one can determine which thread you're on and will 
                                                                       // respond or post and then respond appropriately.


COLstring WEBresponseSerialize(WEBresponse* pResponse);

#endif // end of defensive include
