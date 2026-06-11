#ifndef __RGN_MAC_ADDRESS_NONE_H__
#define __RGN_MAC_ADDRESS_NONE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: RGNMACAddressBSD
//
// Description:
// Implementation of RGNMACAddress for unknown platforms.  
// Simply returns an empty string
//
// Author: George Ma 
// Date:   03/17/2004
//
//---------------------------------------------------------------------------

#include "RGNobfuscate.h"

COLstring RGNmacAddress()
{
   return "";
}

#endif // end of defensive include

