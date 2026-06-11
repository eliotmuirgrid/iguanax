#ifndef __WEB_MIME_LOOKUP_H__
#define __WEB_MIME_LOOKUP_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBmimeLookup
//
// Description:
//
// A hashmap of mime types. Function to register new extension to map and function
// to look up a mime type given the extension.
//
// Author: Vismay Shah 
// Date:   Friday 02 June 2023 - 11:02AM
// ---------------------------------------------------------------------------
#include <COL/COLstring.h>

void WEBImimeLookupInitMap();
COLstring WEBImimeType(const COLstring& FileExtension);

// In case you want to append to the map after the setup
void WEBImimeLookupAddMimeType(const COLstring& FileExtension, const COLstring& MimeType);

#endif // end of defensive include
