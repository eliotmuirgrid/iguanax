#ifndef __HTTP_MIME_LOOKUP_H__
#define __HTTP_MIME_LOOKUP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPmimeLookup
//
// Description:
//
// HTTPmimeLookup class - HTTP applications can choose whether to use this
// or not.
//
// Author: Eliot Muir
// Date:   Wednesday, December 20th, 2006 @ 09:18:57 AM
//
//---------------------------------------------------------------------------

#include <COL/COLhashmap.h>

class HTTPmimeLookup {
public:
   HTTPmimeLookup();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~HTTPmimeLookup();

   void addMimeType(const COLstring& FileExtension, const COLstring& MimeType);

   COLstring mimeType(const COLstring& FileExtension) const;
private:
   COLhashmap<COLstring, COLstring> m_MimeLookup;
   HTTPmimeLookup(const HTTPmimeLookup& Orig); // not allowed
   HTTPmimeLookup& operator=(const HTTPmimeLookup& Orig); // not allowed
};

#endif // end of defensive include
