//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPmultiPart
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Saturday, December 9th, 2006 @ 03:06:37 PM
//
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPmultiPart.h"
#include "HTTPfile.h"
#include "HTTPrequest.h"
#include "HTTPheader.h"

#include <COL/COLarray.h>
#include <COL/COLhashmap.h>
#include <COL/COLvar.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <string.h>
#include <algorithm>

static const COLstring HTTP_MULTIPART_NAME_TOKEN         = "name=\"";
static const COLstring HTTP_MULTIPART_FILENAME_TOKEN     = "filename=\"";
static const COLstring HTTP_MULTIPART_CONTENT_TYPE_TOKEN = "Content-Type:"; 
static const COLstring HTTP_MULTIPART_HEADER_NAME        = "Content-Type";
static const COLstring HTTP_MULTIPART_BOUNDARY           = "boundary";
static const COLstring HTTP_DEFAULT_CONTENT_TYPE         = "application/octet-stream";


void HTTPpopulateFilePostVarsForPart(const COLint8* pStartOfPart, const COLint8* pEndOfPart, COLvar* pAllTheParts, 
                                     HTTPvarMap* pVariableList,  COLarray<HTTPfile>* pFileList, bool NormalizeKeys = false) {
   COL_FUNCTION(HTTPpopulateFilePostVarsForPart);
   COLvar& AllTheParts = *pAllTheParts;
   // everything up to the CRLFCRLF are the headers for the part
   const COLint8* pPartHeadersEnd = strstr(pStartOfPart, "\r\n\r\n");

   // if there's no CRLFCRLF then this is a malformed part, simply return
   if (!pPartHeadersEnd) {
      return;
   }

   // construct a COLstring for the string portion (the headers)
   COLstring PartHeaders(pStartOfPart, pPartHeadersEnd - pStartOfPart);

   // locate the name of the post value
   COLuint32 NamePosition = PartHeaders.find(HTTP_MULTIPART_NAME_TOKEN, 0);
   if (NamePosition > PartHeaders.size()) {
      return;
   }

   NamePosition += HTTP_MULTIPART_NAME_TOKEN.size();

   COLuint32 NameEndPosition = PartHeaders.find("\"", NamePosition);
   COLstring Name = PartHeaders.substr(NamePosition, NameEndPosition - NamePosition);

   // locate the filename, if there is one
   COLstring FileName;
   COLuint32 FileNamePosition = PartHeaders.find(HTTP_MULTIPART_FILENAME_TOKEN, 0);
   
   if (FileNamePosition <= PartHeaders.size()) {
      FileNamePosition += HTTP_MULTIPART_FILENAME_TOKEN.size();
      COLuint32 FileNameEndPosition = PartHeaders.find("\"", FileNamePosition);
      FileName = PartHeaders.substr(FileNamePosition, FileNameEndPosition - FileNamePosition);
   }

   const COLint8* pPartContent = pPartHeadersEnd + 4 * sizeof(char);

   // if we don't have file name information, then we're not receiving
   // a file - treat the post as a regular string post
   if (FileName.is_null()) {
      COLstring Value(pPartContent, pEndOfPart-pPartContent);
      (*pVariableList)[NormalizeKeys ? Name.toLowerCase() : Name] = Value;
      AllTheParts[Name.toLowerCase()] = Value;
   } else  { // file
      // locate the mime type for the file, if there is one
      COLstring FileType;
      size_t FileTypePosition = PartHeaders.find(HTTP_MULTIPART_CONTENT_TYPE_TOKEN, 0);
      if (FileTypePosition != npos) {
         FileTypePosition += HTTP_MULTIPART_CONTENT_TYPE_TOKEN.size();

         size_t FileTypeEndPosition = PartHeaders.find("\r\n", FileTypePosition);
         if (FileTypeEndPosition != npos) {
            FileType = PartHeaders.substr(FileTypePosition, FileTypeEndPosition - FileTypePosition);      
         }
         else {
            FileType = PartHeaders.substr(FileTypePosition);
         }
         FileType.stripAll(' ');
      }

      // If the file type is missing, we use a default catch-all type (see #17067).
      if (FileType.is_null()){
         FileType = HTTP_DEFAULT_CONTENT_TYPE;
      }
      
      HTTPfile& NewFile = pFileList->push_back();
      NewFile.setName(FileName);
      NewFile.setMimeType(FileType);
      NewFile.setData(pPartContent, pEndOfPart-pPartContent);

      COLvar& OneFile = AllTheParts["files"].push_back();
      OneFile["name"] = FileName;
      OneFile["mime_type"] = FileType;
      OneFile["data"] = COLstring(pPartContent, pEndOfPart-pPartContent);
   }
}

// Temporary all-in-one edition
void HTTPextractMultiPartData(const void* pData, COLuint32 SizeOfData, const COLstring& BoundaryString, COLvar* pAllTheParts, 
                              HTTPvarMap* pVariableList, COLarray<HTTPfile>* pFileList, bool NormalizeKeys) {
   COL_FUNCTION(HTTPextractMultiPartData);
   COLvar& AllTheParts = *pAllTheParts;
   AllTheParts["files"].setArrayType();
   COLint8* pCurrentPositionInBody = (COLint8*)pData;
   COLuint32 BytesLeft = SizeOfData;
   pCurrentPositionInBody = strstr(pCurrentPositionInBody, BoundaryString.c_str());
   if (!pCurrentPositionInBody) {
      return;
   }
   BytesLeft -= pCurrentPositionInBody - (COLint8*)pData;

   // index past the first instance of the boundary pattern
   COLuint32 ByteIncrement = (strlen(BoundaryString.c_str()) * sizeof(char));
   pCurrentPositionInBody += ByteIncrement;
   BytesLeft -= ByteIncrement;

   // now, we need to find the length of each of the parts of
   // this multi-part request - so we search for the boundary
   // condition using mem functions (since binary data is allowed)
   // Then we'll parse each section
   while (pCurrentPositionInBody) {
      COLint8* pCurrentSectionEnd = pCurrentPositionInBody;
      bool LocatedBoundary = false;
      while (!LocatedBoundary) {

         // locate the first character in our boundary condition
         COLint8* pTempCurrentSectionEnd = (COLint8*)memchr(pCurrentSectionEnd, BoundaryString[0], BytesLeft);
         BytesLeft -= (pTempCurrentSectionEnd - pCurrentSectionEnd);

         // if we can't find the first character in our boundary condition, then
         // there is no end of this boundary.. this shouldn't happen, but if it
         // does, we're decidedly done.
         if (!pTempCurrentSectionEnd || (BytesLeft <= 0)) {
            pCurrentPositionInBody = NULL;
            break;
         }
         pCurrentSectionEnd = pTempCurrentSectionEnd;

         // if we have located the end of our current 'part', then determine its length in bytes
         // and parse it for post values
         // pick smaller of bytes left and boundary string so that memcmp doesn't try to read outside of pCurrentSectionEnd
         if (memcmp(pCurrentSectionEnd, BoundaryString.c_str(), ( std::min(BytesLeft, (unsigned int) BoundaryString.size()))) == 0) {
            LocatedBoundary = true;

            // locate the \r\n somewhere just before the 
            // boundary, this is the actual end of the part
            COLint8* pCurrentSectionContentEnd = pCurrentSectionEnd;
            while (memcmp(pCurrentSectionContentEnd, "\r\n", 2) != 0) {
               pCurrentSectionContentEnd--;
            }

            HTTPpopulateFilePostVarsForPart(pCurrentPositionInBody, pCurrentSectionContentEnd, pAllTheParts, pVariableList, pFileList, NormalizeKeys);
            pCurrentPositionInBody = pCurrentSectionEnd + ByteIncrement;
            BytesLeft -= ByteIncrement;
         }

         // if this isn't the end of our current part, index past the current character 
         // and keep on looking
         else {
            pCurrentSectionEnd++;
            BytesLeft--;
         }
      } // end loop that deals with locating a boundary for a specific part

   } // end loop over all parts
   COL_VAR(AllTheParts.json(true));
}

// Old version
void HTTPextractMultiPartData(const void* pData, COLuint32 SizeOfData,   // 32-bit limit ... good enough
                               const COLstring& BoundaryString, HTTPvarMap* pVariableList, COLarray<HTTPfile>* pFileList, bool NormalizeKeys) {
   COL_FUNCTION(HTTPextractMultiPartData);
   COLvar Fake;
   HTTPextractMultiPartData(pData, SizeOfData, BoundaryString, &Fake, pVariableList, pFileList, NormalizeKeys);
}

// New version
void HTTPextractMultiPartData(const void* pData, COLuint32 SizeOfData, const COLstring& BoundaryString, COLvar* pAllTheParts) {
   COL_FUNCTION(HTTPextractMultiPartData);
   HTTPvarMap Fake1;
   COLarray<HTTPfile> Fake2;
   HTTPextractMultiPartData(pData, SizeOfData, BoundaryString, pAllTheParts, &Fake1, &Fake2, true);
}

COLstring HTTPgetBoundary(const HTTPrequest& FullRequest) {
   const HTTPheader& Request = FullRequest.header();
   COLuint32 HeaderIndex = Request.findHeader(HTTP_MULTIPART_HEADER_NAME);
   if (HeaderIndex != npos) {
      const COLstring BoundaryValue = Request.headerValue(HeaderIndex);
      size_t BoundaryIndex = BoundaryValue.find(HTTP_MULTIPART_BOUNDARY, 0);
      if (BoundaryIndex != npos) {
         return BoundaryValue.substr(BoundaryIndex + HTTP_MULTIPART_BOUNDARY.size() + 1);
      }
   }
   return COLstring();
}
