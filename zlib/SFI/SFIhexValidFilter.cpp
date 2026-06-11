//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIhexValidFilter
//
// Description:
//
// Implementation.
//
// Author: Henry Tran
// Date: July 6, 2004
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include "SFIhexValidFilter.h"

COLuint32 SFIhexValidFilter::write(const void* pBuffer, COLuint32 SizeOfBuffer)
{
   const COLuint32 OriginalSizeOfBuffer = SizeOfBuffer;

   while ( SizeOfBuffer > 0 )
   {
      unsigned char Character = *((unsigned char*)pBuffer);
      if ( (Character >= '0' && Character <= '9')
        || (Character >= 'A' && Character <= 'F')
        || (Character >= 'a' && Character <= 'f'))
      {
         next().write( pBuffer, 1 );
      }

      if ( SizeOfBuffer > 1 )
      {
         pBuffer = (unsigned char*) pBuffer + 1;
      }
      SizeOfBuffer -= 1;
   }
   return OriginalSizeOfBuffer;
}

