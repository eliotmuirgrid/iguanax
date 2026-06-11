#ifndef __SFI_BASE64_H__
#define __SFI_BASE64_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIbase64
//
// Description:
//
// Used for base 64 conversion of a string.
//
// Author: Rob Moyse
// Date: July 15, 2004 
//
//---------------------------------------------------------------------------


class SFIbase64{
public:

   static const char* BASE64_CHARS;

   // returns the encoded null-terminated string
   static COLstring encode(const COLuint8* pSourceBuffer, COLuint32 BufferLength);

   // assumes encoded pSourceBuffer is null terminated and returns 
   // a buffer containing the decoded data 
   static COLstring decode(const char* pSourceBuffer);

   //writes to the provided simplebuffer the base64 encoded value. does not null terminate
   static void encodeNonTerminated(const COLuint8* pSourceBuffer, COLuint32 BufferLength, COLstring& OutputBuffer);

   //writes to the provided simplebuffer the base64 decoded value. does not null terminate
   static void decodeNonTerminated(const COLuint8* pSourceBuffer, COLuint32 BufferLength, COLstring& OutputBuffer);

   static void encodeTriple(const COLuint8 Triple[3], COLuint8 Result[4]);  

private:

   static COLuint32 decodeTriple (char Quadruple[4], COLuint8* TripleResult);
   static COLint32 characterValue(char Base64Character);

   SFIbase64(); // not allowed
};
#endif // end of defensive include
