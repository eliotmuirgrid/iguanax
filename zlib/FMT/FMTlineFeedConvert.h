#ifndef __FMT_LINE_FEED_CONVERT_H__
#define __FMT_LINE_FEED_CONVERT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FMTlineFeedConvert.h
//
// Description:
//
// This class converts a string with just \0D characters to use
// complete \0D \0A sequences so that it can be output using standard
// ostream operators.
//
// Author: Eliot Muir
// Date:   Tuesday, March 21st, 2006 @ 11:42:48 AM
//
//---------------------------------------------------------------------------

#include <COL/COLstring.h> // duh no envelope pattern...

class COLostream;

class FMTlineFeedConvert
{
public:
   FMTlineFeedConvert(const char* pString, const char* LineFeed);
   FMTlineFeedConvert(const FMTlineFeedConvert& Original);
   FMTlineFeedConvert& operator=(const FMTlineFeedConvert& Original);
   ~FMTlineFeedConvert() {}
   
   void PrintOn(COLostream& Stream) const;
private:
   const char* m_pString;
   COLstring m_pLineFeed;
};
   
COLstring FMTmakeMessagePrintable(const COLstring& InputString, const COLstring& LineFeed, COLuint32 MaximumLength);

COLostream& operator<<
(
   COLostream& Stream, 
   const FMTlineFeedConvert& Formatter
);
#endif // end of defensive include
