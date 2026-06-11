#ifndef __TXT_ENCODING_FILTER_H__
#define __TXT_ENCODING_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TXTencodingFilter
//
// Description:
//
// TXTencodingFilter provides a COLfilter implementation of text encoding
// conversion. It is less forgiving than TXTencoding, which does some recovery
// on illegal byte sequences (by substituting with placeholders).
// The filter will throw if an illegal byte sequence is encountered.
// All the underlying work is done with the iconv library.
//
// Author: Akshay Ganeshen
// Date:   Thu  4 Dec 2014 17:14:40 EST
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLfilter.h>
#include <COL/COLstring.h>

#include <ICONV/iconv.h>

class TXTencodingFilter : public COLfilter {
public:
   TXTencodingFilter(COLsink* pNext, const COLstring& FromEncoding, const COLstring& ToEncoding);
   TXTencodingFilter(COLsink* pNext, const COLstring& FromEncoding /* ToEncoding = "UTF8" */);
   virtual ~TXTencodingFilter();

   // It's a good idea to call isSupported before trying to write, or write will throw immediately
   bool isSupported() const;

   // Reads data in from a pointer to the buffer, and writes the converted encoding to pNext - can throw
   virtual COLuint32 write(const void* cpBuffer, COLuint32 SizeOfBuffer);

   // Allows resetting the filter, which just destroys and reinitializes the iconv handle and clears the leftover buffer
   virtual void resetFilter();

   int unprocessedCharacterCount() const;
   // Overload the default onEndStream to assert that no leftover conversions are waiting to happen
   virtual void onEndStream();
   // Use the default flush - next().flush() - write already handles all the data at once
private:
   void initIconvHandle(const COLstring& FromEncoding, const COLstring& ToEncoding);
   void freeIconvHandle();

   iconv_t m_IconvHandle;
   // Keep a backup of the from/to encodings so the filter supports resetting
   COLstring m_FromEncoding, m_ToEncoding;

   // And finally, an internal buffer that holds any leftover bytes that may remain from converting
   COLstring m_LeftoverBuffer;

   TXTencodingFilter(TXTencodingFilter& Orig); // not allowed
   TXTencodingFilter& operator=(TXTencodingFilter& Orig); // not allowed
};

// These throw on invalid/unsupported inputs
COLstring TXTencode(void* pBuffer, int Size, const COLstring& FromEncoding, const COLstring& ToEncoding);
COLstring TXTencode(const COLstring& Data, const COLstring& FromEncoding, const COLstring& ToEncoding);

#endif // end of defensive include
