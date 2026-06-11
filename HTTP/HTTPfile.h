#ifndef __HTTP_FILE_H__
#define __HTTP_FILE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPfile
//
// Description:
//
// HTTPfile class - this is a simple POD class used to locate each
// file in the buffer for multi part mime HTTP request.
//
// Author: Eliot - this is different now.
// Date:   Thursday, December 7th, 2006 @ 03:27:52 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class HTTPfile
{
public:
   HTTPfile() {}
   ~HTTPfile() {}

   //POD so default copy constructor and assignment operator=
   //work just fine.

   const void* data() const;
   COLuint32 size() const;

   // This does *not* take ownership
   void setData(const void* pStart, COLuint32 SizeOfData);

   const COLstring& name() const;
   void setName(const COLstring& Name);

   const COLstring& mimeType() const;
   void setMimeType(const COLstring& MimeType);
private:
   const void* pData;
   COLuint32 SizeOfData;  // 32 bit limit... should be okay...?
   COLstring Name;
   COLstring MimeType;
};

#endif // end of defensive include
