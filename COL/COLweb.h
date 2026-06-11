#ifndef __COL_WEB_H__
#define __COL_WEB_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: COLweb
//
// Author: Matthew Sobkowski and Eliot Muir
//
// Common boilerplate for WEB handler functions.
//---------------------------------------------------------------------------
#include <COL/COLmap.h>
#include <COL/COLclosure.h>
#include <COL/COLauto.h>
#include <COL/COLvar.h>

class COLvar;

// This is meant to be a lightweight address data structure, passed to the responder functions so they
// know which address to respond to. Right now its just an integer representing a socket file descriptor,
// but can be extended to handle internal and grid communication.
class COLaddress{
public:
   COLuint64 ConnectionId=0;
   COLclosure1<const COLvar&>* pInternalResponseCallback=NULL;
};

// This class is an awkward compromise between having flexibility, but standardization of the incoming data.
// GET/POST variables should be parsed into Data.
// There isn't the ability to distinguish between them.  Nor will the data be nested within the COLvar - goodbye ["data"]
// The ConnectionId is important enough to warrant it's own concrete variable.  Likewise User.  Please talk
// things through with me before adding more to this object though.  Eliot
class COLwebRequest{
public:
   COLstring Path;    // request URL path
   COLvar Data;       // Contains JSON body and GET variables if they were part of the request.
   COLaddress Address;
   bool IsSecure;
   bool AdminUser = false;
   bool FromTranslator = false;
   COLstring User;    // This should have the user. If FromTranslator is true, this is the component id
   COLstring SessionId;
};

COLostream& operator<<(COLostream& Stream, const COLwebRequest& Request);

typedef void (*COLwebHandlerFunc)(const COLwebRequest& Request);

// Used in DBDinit to hookup pCOLwebResponse, which is used by the COLrespond* functions
void COLwebSetResponseCallback(COLclosure2<COLuint64, const COLstring&>* pCallback);
void COLsendWebResponse(COLuint64 ConnectionId, const COLstring& Data);

// One line functions to make a response using COLaddress to send it to the appropriate location. Can be called from
// the main thread or a worker thread.
void COLrespondError  (const COLaddress& Address, const COLstring& Error);
void COLrespondError  (const COLaddress& Address, const COLstring& ErrorMessage, const COLvar& ErrorVar);
void COLrespondSuccess(const COLaddress& Address);
void COLrespondSuccess(const COLaddress& Address, const COLvar& Data);

// I use this in the TUNnel client.
void COLwebJsonResponse(const COLvar& Data, COLstring* pHttpOut);

void COLrouteResponse(const COLvar& Result, const COLaddress& Address);

#endif // end of defensive include
