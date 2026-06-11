//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIunixToUnix
//
// Description:
//
// Implementation
//
// Author: Eric Mulvaney
// Date:   Tuesday, January 10th, 2012 @ 11:31:16 AM
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#define SFI_NEED_UU_DEFINES
#include "SFIunixToUnix.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#undef ERROR
#define ERROR(X) \
   do{ COL_ERROR_STREAM_PLAIN(X, COLerror::PreCondition); }while(0)

inline int uuenc(int x) { return (x & 077) ? (x & 077) + ' ' : '`'; }
inline int uudec(int x) { return (x - ' ') & 077; }

//
// Encoding
//

void SFIunixToUnixEncode::encode(int n)
{
   char OutBuf[UUENCSZ(sizeof m_Buf) + 2];
   char *pOut = OutBuf, *pIn = m_Buf, *pEnd = m_Buf+n;
   for(int i = (3-n%3)%3; i--;) *pEnd++ = 0;  // Zero-pad input if needed.
   *pOut++ = uuenc(n);  // Do not include padding in size byte.
   for(; pIn < pEnd; pIn += 3) {
      // 1111-1122 2222-3333 3344-4444
      *pOut++ = uuenc(pIn[0] >> 2);
      *pOut++ = uuenc((pIn[0] << 4) | ((pIn[1] >> 4) & 017));
      *pOut++ = uuenc((pIn[1] << 2) | ((pIn[2] >> 6) & 003));
      *pOut++ = uuenc(pIn[2]);
   }
   *pOut++ = '\n';
   next().write(OutBuf, pOut - OutBuf);
}

void SFIunixToUnixEncode::writeHeader()
{
   char Buf[32];
   next().write(Buf, snprintf(Buf, 32, "begin %o ", m_Mode));
   next().write(m_Name.c_str(), m_Name.size());
   next().write("\n", 1);
   m_Started = true;
}

COLuint32 SFIunixToUnixEncode::write(const void* pVoid, COLuint32 Size)
{
   const char* pIn = (const char*)pVoid;
   int Left = Size;
   if(!m_Started) writeHeader();
   while(m_BufSize + Left > sizeof m_Buf) {
      int n = sizeof m_Buf - m_BufSize;
      memcpy(m_Buf + m_BufSize, pIn, n);
      encode(sizeof m_Buf);
      pIn += n; Left -= n;
      m_BufSize = 0;
   }
   COLASSERT(Left >= 0);
   memcpy(m_Buf + m_BufSize, pIn, Left);
   m_BufSize += Left;
   return Size;
}

void SFIunixToUnixEncode::onEndStream()
{
   if(!m_Started) writeHeader();
   if(m_BufSize) encode(m_BufSize);
   encode(0);
   next().write("end\n", 4);
}

//
// Decoding
//

int SFIunixToUnixDecode::decode(int InSize)
{
   char OutBuf[077]; char *pOut = OutBuf;
   const char *pIn = m_Buf, *pEnd = m_Buf + InSize;
   if(InSize < 1) return -1;  // Ignore empty lines.
   int n = uudec(*pIn++);
   if(InSize < UUMINENCSZ(n) + 1) {
      ERROR("Short line in UUEncoded file (line " << m_LineNo << ").");
   }
   for(int i=0; i < n; i += 3) {
      // xx11-1111 xx11-2222 xx22-2233 xx33-3333
      int c0 = uudec(*pIn++); int c1 = uudec(*pIn++);
      int c2 = uudec(*pIn++); int c3 = uudec(*pIn++);
      *pOut++ = c0 << 2 | ((c1 >> 4) & 003);
      *pOut++ = c1 << 4 | ((c2 >> 2) & 017);
      *pOut++ = c2 << 6 | (c3        & 077);
   }
   next().write(OutBuf, n);
   return n;
}

bool SFIunixToUnixDecode::readHeader(const char*& pIn, int& Size)
{
   for(;;) {
      const char* pEnd = (const char*)memchr(pIn, '\n', Size);
      const int Max = 1024 - m_Header.size();  // Avoid heap bombs.
      if(!pEnd) {
         m_Header.append(pIn,COL_MIN(Size,Max));
         pIn += Size; Size = 0;
         break;
      } else {
         int n = pEnd - pIn;
         m_Header.append(pIn,COL_MIN(n,Max));
         m_LineNo++;
         pIn += n + 1; Size -= n + 1;
         int Mode; unsigned char Gap;
         if(sscanf(m_Header.c_str(), "begin %o%c%n", &Mode, &Gap, &n) < 2 || !COLisspace(Gap)) {
            m_Header.clear();  // Ignore garbage lines.
         } else {
            if(m_pInfo) {
               m_pInfo->Mode     = Mode;
               m_pInfo->FileName = m_Header.substr(n).strip();
            }
            m_Header.zero();
            m_Ready = true;
            break;
         }
      }
   }
   return m_Ready;
}

COLuint32 SFIunixToUnixDecode::write(const void* pVoid, COLuint32 Size)
{
   const char* pIn = (const char*)pVoid;
   int Left = Size;
   if(!m_Ready && !readHeader(pIn, Left)) return Size;
   for(;;) {
      const char* pEnd = (const char*)memchr(pIn, '\n', Left);
      if(!pEnd) {
         int Max = sizeof m_Buf - m_BufSize;
         int n = COL_MIN(Left, Max);
         memcpy(m_Buf + m_BufSize, pIn, n);
         m_BufSize += n;
         break;
      } else {
         int Max = sizeof m_Buf - m_BufSize;
         int n = pEnd - pIn; if(n > Max) n = Max;
         memcpy(m_Buf + m_BufSize, pIn, n);
         m_LineNo++;
         pIn += n + 1; Left -= n + 1;
         if(!m_Finished) {
            m_Finished = !decode(m_BufSize + n);
         } else if(!memcmp(m_Buf, "begin ", 6)) {
            ERROR("Multiple UUEncoded files in input (line " << m_LineNo << ").");
         }
         m_BufSize = 0;
      }
   }
   return Size;
}

void SFIunixToUnixDecode::onEndStream()
{
   if(!m_Ready) {
      ERROR("No UUEncoded file found.");
   } else if(!m_Finished) {
      ERROR("Unexpected end of UUEncoded file.");
   }
}
