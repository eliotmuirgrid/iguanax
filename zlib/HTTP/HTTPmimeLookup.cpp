//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPmimeLookup
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, December 20th, 2006 @ 09:18:57 AM
//
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPmimeLookup.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

HTTPmimeLookup::HTTPmimeLookup(){
   COL_METHOD(HTTPmimeLookup::HTTPmimeLookup);
   // copy pasted basic mime types from Rob's HTP library. :-) 
   addMimeType("html", "text/html");
   addMimeType("htm", "text/html");
   addMimeType("txt", "text/plain");
   addMimeType("lua", "text/plain");
   addMimeType("css", "text/css");
   addMimeType("xml", "text/xml");
   addMimeType("gif", "image/gif");
   addMimeType("jpg", "image/jpeg");
   addMimeType("jpeg", "image/jpeg");
   addMimeType("bmp", "image/x-xbitmap");
   addMimeType("png", "image/png");
   addMimeType("js", "text/javascript");
   addMimeType("dbs", "text/plain");
   addMimeType("sql", "text/plain");
   addMimeType("vmd", "application/chameleon"); // this matches the type we set in the Windows registry.
   addMimeType("woff", "font/woff");
   addMimeType("woff2", "font/woff2");
   addMimeType("ico", "image/x-icon");
   addMimeType("ttf", "application/x-font-truetype");
   addMimeType("svg", "image/svg+xml");
   addMimeType("exe", "application/x-msdownload");
   addMimeType("pdf", "application/pdf");
   addMimeType("eml", "message/rfc822");
}

HTTPmimeLookup::~HTTPmimeLookup(){
   COL_METHOD(HTTPmimeLookup::~HTTPmimeLookup);
}

void HTTPmimeLookup::addMimeType(const COLstring& FileExtension, const COLstring& MimeType){
   COL_METHOD(HTTPmimeLookup::addMimeType);
   COL_VAR2(FileExtension, MimeType);
   m_MimeLookup[FileExtension] = MimeType;
}

COLstring HTTPmimeLookup::mimeType(const COLstring& FileExtension) const{
   COL_METHOD(HTTPmimeLookup::mimeType);
   COLhashmapPlace MimeIndex = m_MimeLookup.find(FileExtension);
   if (MimeIndex){
      COL_VAR2(FileExtension, m_MimeLookup[MimeIndex]);
      return m_MimeLookup[MimeIndex];
   } else {
      COL_ERR("Unknown extension '" << FileExtension << "'- returning text/plain"); 
      // We return the type as text/html - TODO there might be a more general form we could return.
      return "text/plain";
   }
}
