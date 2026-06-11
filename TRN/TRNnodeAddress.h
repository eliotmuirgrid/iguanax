#ifndef __TRN_NODE_ADDRESS_H__
#define __TRN_NODE_ADDRESS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNnodeAddress
//
// Description:
//
// TRNnodeAddress - code for formatting node addresses
//
// Author: Eliot Muir
// Date:   Thursday, November 25th, 2010 @ 10:51:59 PM
//---------------------------------------------------------------------------

class NODaddress;
class NODplace;
class COLstring;
class COLostream;

const NODplace& TRNoutputTruncatedAddress(COLostream& Stream, const COLstring& SourceName, const NODaddress& LastAddress, const NODaddress& CurrentAddress);

#endif // end of defensive include
