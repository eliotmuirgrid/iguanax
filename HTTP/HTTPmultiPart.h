#ifndef __HTTP_MULTI_PART_H__
#define __HTTP_MULTI_PART_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPmultiPart
//
// Description:
//
// Routine to parse out file and post variables from a multi-part MIME
// HTTP request.  The current implementation does have some limitations
// in terms of that it won't scale really well for very large files.  That
// could be done if the need arises later.  The nice thing is that an application
// writer shouldn't need to alter the HTTP library if they have that requirement
// since this library is much more loosely coupled than previous HTTP libraries
// we've had.
//
// Author: Eliot Muir
// Date:   Saturday, December 9th, 2006 @ 03:06:37 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <HTTP/HTTPvariables.h>

class HTTPfile;
class HTTPrequest;

template <class TItem> class COLarray;

template <class KeyType>
struct COLhash;

template <class TKey, class TValue, class CompareType >
class COLhashmap;

void HTTPextractMultiPartData(const void* pData, COLuint32 SizeOfData,   // 32 limit ... good enough - this implementation won't scale to really large files anyway
                               const COLstring& BoundaryString, HTTPvarMap* pVariableList, COLarray<HTTPfile>* pFileList, bool NormalizeKeys = false);
void HTTPextractMultiPartData(const void* pData, COLuint32 SizeOfData, const COLstring& BoundaryString, COLvar* pAllTheParts);

// Temporary all-in-one version
void HTTPextractMultiPartData(const void* pData, COLuint32 SizeOfData, const COLstring& BoundaryString, COLvar* pAllTheParts, 
                              HTTPvarMap* pVariableList, COLarray<HTTPfile>* pFileList, bool NormalizeKeys = false);

COLstring HTTPgetBoundary(const HTTPrequest& Header);

#endif // end of defensive include
