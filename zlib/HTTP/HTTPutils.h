#ifndef __HTTP_UTILS_H__
#define __HTTP_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPutils
//
// Description:
//
// Utility routines for dealing with HTTP
//
// Author: Eliot Muir
// Date:   Wednesday, December 6th, 2006 @ 10:38:49 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <HTTP/HTTPvariables.h>

class COLostream;
class HTTPrequest;

template <class KeyType>
struct COLhash;
template <class TKey, class TValue, class CompareType >
class COLhashmap;

void HTTPdumpRequest(COLostream& Stream, const HTTPrequest& Request, const void* pBodyData, COLuint32 SizeOfBody);

// Extracts all POST variables, then GET variables into a single hashtable - note that this means
// GET variables will over-write POST variables with the same name.
void HTTPextractAllVariables(HTTPvarMap* pVariableTable, const HTTPrequest& Request, const void* pBodyData, COLuint32 SizeOfBody, bool NormalizeKeys = false);

// See comments in implementation about the limitation of this routine
void HTTPshowParseErrorLocation(const char* pData, COLuint32 SizeOfData, COLuint32 ErrorLocation, COLostream& Stream);


// Generates a plain error response - can be handy.
COLstring HTTPgeneratePlainErrorResponse(const COLstring& ErrorMessage);

// This function makes a set of hidden input fields that can
// be used to implement a login screen that redirects to the screen the customer
// was wanting.
COLstring HTTPserializeRequestToForm(
   const COLstring& Location, 
   const COLhashmap<COLstring, COLstring, COLhash<COLstring> >& VariableList
);

// This is the reverse of the above function
void HTTPdeserializeRequestFromForm(
   const COLhashmap<COLstring, COLstring, COLhash<COLstring> >& RequestVariables,
   COLstring& Location,
   COLhashmap<COLstring, COLstring, COLhash<COLstring> >& VariableList
);

// Parse a URL 
// Returns false if the URL is not valid
// *pHost will contain the Bad data.
bool HTTPparseUrl(const COLstring& Url, bool* pIsHTTPS, COLstring* pHost, COLstring* pPath, int* pPort);

#endif // end of defensive include
