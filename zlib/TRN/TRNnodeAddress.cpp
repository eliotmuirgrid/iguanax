//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNnodeAddress
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday, November 25th, 2010 @ 10:51:59 PM
//---------------------------------------------------------------------------
#include "TRNnodeAddress.h"

#include <NOD/NODaddress.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

const NODplace& TRNoutputTruncatedAddress(COLostream& Stream, const COLstring& SourceName, const NODaddress& LastAddress, const NODaddress& CurrentAddress){
   COL_FUNCTION(TRNoutputTruncatedAddress);
   if (LastAddress.parent() != CurrentAddress.parent()){
      COL_TRC("Parent doesn't match so no greying of annotation.");
      return NODoutputAddress(Stream, SourceName, CurrentAddress);
   }
   const NODplace* pLeafNode = CurrentAddress.parent();

   int NodeCount = COL_MIN(LastAddress.depth(), CurrentAddress.depth());
   int i = 0;
   int BlankCount = 0;

   Stream << "<span class=\'gr\'>" << SourceName;

   bool AddressMatchingOldAddress = true;

   for (i = 0; i < CurrentAddress.depth(); i++){
      if (AddressMatchingOldAddress && i < LastAddress.depth() && LastAddress.addressIndex(i) != CurrentAddress.addressIndex(i)){
         AddressMatchingOldAddress = false;
         Stream << "</span>";
      }
      pLeafNode = &NODoutputAddressIndex(Stream, CurrentAddress.addressIndex(i), *pLeafNode);    
   }
   if (AddressMatchingOldAddress) Stream << "</span>";
 
   return *pLeafNode;
}
