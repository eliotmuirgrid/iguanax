// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBImimeLookup
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 02 June 2023 - 11:02AM
// ---------------------------------------------------------------------------

#include <WEBI/WEBImimeLookup.h>

#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLhashmap<COLstring, COLstring> WEBmimeLookup;

void WEBImimeLookupInitMap(){
   COL_FUNCTION(WEBImimeLookupInitMap);
   // copy pasted basic mime types from Rob's HTP library. :-) 
   WEBmimeLookup["html"]  =   "text/html";
   WEBmimeLookup["htm"]   =  "text/html";
   WEBmimeLookup["txt"]   =  "text/plain";
   WEBmimeLookup["lua"]   =  "text/plain";
   WEBmimeLookup["css"]   =  "text/css";
   WEBmimeLookup["xml"]   =  "text/xml";
   WEBmimeLookup["gif"]   =  "image/gif";
   WEBmimeLookup["jpg"]   =  "image/jpeg";
   WEBmimeLookup["jpeg"]  =   "image/jpeg";
   WEBmimeLookup["bmp"]   =  "image/x-xbitmap";
   WEBmimeLookup["png"]   =  "image/png";
   WEBmimeLookup["js"]    =  "text/javascript";
   WEBmimeLookup["dbs"]   =  "text/plain";
   WEBmimeLookup["sql"]   =  "text/plain";
   WEBmimeLookup["vmd"]   =  "application/chameleon"; // this matches the type we set in the Windows registry.
   WEBmimeLookup["woff"]  =   "font/woff";
   WEBmimeLookup["woff2"] =   "font/woff2";
   WEBmimeLookup["ico"]   =  "image/x-icon";
   WEBmimeLookup["ttf"]   =  "application/x-font-truetype";
   WEBmimeLookup["svg"]   =  "image/svg+xml";
   WEBmimeLookup["exe"]   =  "application/x-msdownload";
}


void WEBImimeLookupAddMimeType(const COLstring& FileExtension, const COLstring& MimeType){
   COL_FUNCTION(WEBImimeLookupAddMimeType);
   COL_VAR2(FileExtension, MimeType);
   WEBmimeLookup[FileExtension] = MimeType;
}

COLstring WEBImimeType(const COLstring& FileExtension){
   COL_FUNCTION(WEBImimeType);
   COLhashmapPlace MimeIndex = WEBmimeLookup.find(FileExtension);
   if (MimeIndex){
      COL_VAR2(FileExtension, WEBmimeLookup[MimeIndex]);
      return WEBmimeLookup[MimeIndex];
   } else {
      COL_ERR("Unknown extension '" << FileExtension << "'- returning text/plain"); 
      // We return the type as text/html - TODO there might be a more general form we could return.
      return "text/plain";
   }
}
