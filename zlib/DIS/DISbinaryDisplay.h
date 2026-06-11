#ifndef __DIS_BINARY_DISPLAY_H__
#define __DIS_BINARY_DISPLAY_H__
//---------------------------------------------------------------------------
// Module: DISbinaryDisplay
//
// Copyright (C) 1997-2010 iNTERFACEWARE Inc. All Rights Reserved
//
// Description:
//
// This object nicely formats binary data in an HTML format.
//
// Author: Eliot Muir
// Date:   Sun 09/29/2002 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class DISbinaryDisplayPrivate;

class DISbinaryDisplay 
{
public:
   DISbinaryDisplay();
   ~DISbinaryDisplay();
   
   // TODO - use COLuint32 instead of int etc.
   static void displayData(COLostream& Stream,    
                           const char* pBuffer, 
                           int         Length,                     
                           int         ErrorIndexMin=-1,
                           int         ErrorIndexMax=-1,
                           bool  AsHtml=true,
                           int         DisplayStartByteOffset = 0,
                           int         TotalStreamSize = -1
                          );

private:
   DISbinaryDisplayPrivate* pMember;

   DISbinaryDisplay(const DISbinaryDisplay& Orig);
   DISbinaryDisplay& operator=(const DISbinaryDisplay& Orig);
};

#endif // end of defensive include

