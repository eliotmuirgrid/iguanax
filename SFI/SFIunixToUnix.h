#ifndef __SFI_UNIX_TO_UNIX_H__
#define __SFI_UNIX_TO_UNIX_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIunixToUnix
//
// Description:
//
// Unix to Unix Encoding Filters
//
// SFIunixToUnixEncode - Works like the uuencode(1) utility.
// SFIunixToUnixDecode - Works like the uudecode(1) utility.
//
// Author: Eric Mulvaney
// Date:   Tuesday, January 10th, 2012 @ 11:31:16 AM
//
//---------------------------------------------------------------------------

#include <COL/COLfilter.h>
#include <COL/COLstring.h>
#include <COL/COLtrackable.h>
#include <COL/COLtracker.h>

#define UUENCSZ(X) ((((X)+2)/3)*4)  // Encoded size of X bytes (ideal).
#define UUMINENCSZ(X) (UUENCSZ(X) - (3-(X)%3)%3)  // Minimum encoded size.

class SFIunixToUnixEncode : public COLfilter
{
   const int       m_Mode;
   const COLstring m_Name;
   char m_Buf[45];  // Encodes to 60 bytes (+2 for a line).
   int  m_BufSize;  // The number of input bytes in m_Buf.
   bool m_Started;  // True if we've already written something out.
   void encode(int);
   void writeHeader();
   COLuint32 write(const void*, COLuint32);
   void onEndStream();
   void resetFilter() {}
   SFIunixToUnixEncode(const SFIunixToUnixEncode&);
   SFIunixToUnixEncode& operator = (const SFIunixToUnixEncode&);
public:
   SFIunixToUnixEncode(int Mode, const COLstring& Name)
      : COLfilter(NULL), m_Mode(Mode), m_Name(Name), m_BufSize(0)
      , m_Started(false) {}
};

class SFIunixToUnixDecodeInfo : public COLtrackable {
public:
   int       Mode;
   COLstring FileName;
   SFIunixToUnixDecodeInfo() : Mode(0) {}
};

class SFIunixToUnixDecode : public COLfilter, public COLtracker
{
   SFIunixToUnixDecodeInfo* m_pInfo;  // Set when header is parsed.
   COLstring m_Header;          // Buffer used when reading the header.
   char m_Buf[UUENCSZ(077)+1];  // Input line buffer (the useful part).
   int  m_BufSize;              // The number of stale bytes in m_Buf.
   int  m_LineNo;               // The number of the current line.
   bool m_Ready;                // The header has been parsed.
   bool m_Finished;             // The trailer has been parsed.
   int decode(int);
   bool readHeader(const char*&, int&);
   COLuint32 write(const void*, COLuint32);
   void onEndStream();
   void resetFilter() {}
   void onTrackableDestroy(COLtrackable*) { m_pInfo = NULL; }
   SFIunixToUnixDecode(const SFIunixToUnixDecode&);
   SFIunixToUnixDecode& operator = (const SFIunixToUnixDecode&);
public:
   explicit SFIunixToUnixDecode(SFIunixToUnixDecodeInfo* pInfo = NULL)
      : COLfilter(NULL), m_pInfo(pInfo), m_BufSize(0), m_LineNo(0)
      , m_Ready(false), m_Finished(false)
                          { if(m_pInfo) m_pInfo->addTracker(this); }
   ~SFIunixToUnixDecode() { if(m_pInfo) m_pInfo->removeTracker(this); }
};

#ifndef SFI_NEED_UU_DEFINES
#  undef UUENCSZ
#  undef UUMINENCSZ
#endif

#endif // end of defensive include
