#ifndef __HTTP_VARIABLES_H__
#define __HTTP_VARIABLES_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPvariables
//
// Description:
//
// Non member functions for parsing HTTP variables from an HTTP request.
//
// Author: Eliot Muir
// Date:   Saturday, December 9th, 2006 @ 11:22:38 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class HTTPrequest;

template <class KeyType>
struct COLhash;

template <class TKey, class TValue, class CompareType >
class COLhashmap;

typedef COLhashmap<COLstring, COLstring, COLhash<COLstring> > HTTPvarMap;
class COLvar;

void HTTPextractGetVariables(const HTTPrequest& Request, HTTPvarMap* pVariableList, bool NormalizeKeys = false);
void HTTPextractGetVariables(const HTTPrequest& Request, COLvar* pVariableList);

// Extract post variables from body
void HTTPextractPostVariables(const void* pBody, COLuint32 SizeOfBody, HTTPvarMap* pVariableList, bool NormalizeKeys = false);
void HTTPextractPostVariables(const void* pBody, int SizeOfBody, COLvar* pVariableList);

// As above, but it avoids copying the input data.  This is safe inside Iguana
// or whenever requests come from WEBserverConnection and are NUL terminated
// strings.  If ((char*)pBody)[SizeOfBody] is not NUL, bad things will happen.
void HTTPextractPostVariablesFast(const void* pBody, COLuint32 SizeOfBody, HTTPvarMap* pVariableList, bool NormalizeKeys = false);

//Parse HTTP variables into key-value pairs returned in the provided hashmap.
// The default is to use the keys as-is, but if NormalizeKeys is true, then
// the keys will be written out lower-case.
void HTTPparseVariables(const COLstring& RawData, HTTPvarMap* pVariableList, bool NormalizeKeys);
void HTTPparseVariables(const COLstring& RawData, COLvar* pVariableList);  // We lowercase all keys.

enum HTTPrequestType{
   HTTP_GET, 
   HTTP_HEAD,
   HTTP_POST,
   HTTP_POST_MULTIPART,  // multi-part file upload form
   HTTP_OTHER
};

HTTPrequestType HTTPextractRequestType(const HTTPrequest& Request);

void HTTPparseVariables(const HTTPrequest& Request, const void* pBody, int BodySize, COLvar* pVariables);

#endif // end of defensive include
