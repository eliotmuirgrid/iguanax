//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTVtreeValue
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, November 9th, 2010 @ 03:04:01 PM
//
//---------------------------------------------------------------------------
#include "NTVtreeValue.h"
#include "NTVhl7TreeValue.h"

#include <NOD/NODplace.h>

#include <NODB/NODBconstant.h>
#include <NODB/NODBtreeValue.h>
#include <NOX/NOXconstant.h>
#include <NOX/NOXtreeValue.h>
#include <NTAB/NTABconstant.h>
#include <NTAB/NTABtreeValue.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// IX-974 This doesn't feel like the best design since NODplace doesn't really encompass the only place to implement a NODe tree.
void NTVtreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint){
   switch (Place.protocolType()){
      case NODB_DB:
         NODBtreeValue(Stream, Place, TRNhtmlPrint);
         break;
      case NOX_XML:
         NOXtreeValue(Stream, Place, TRNhtmlPrint);
         break;
      case NTAB_TABLE_PROTOCOL:
         NTABtreeValue(Stream, Place, TRNhtmlPrint);
         break;
      case 101:  // Code horror from Eliot 
         NTVhl7TreeValue(Stream, Place, TRNhtmlPrint);
         break;
      default:
         COLASSERT(false);
   }
}

