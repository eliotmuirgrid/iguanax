// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNhash
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Saturday 24 June 2023 - 11:59AM
// ---------------------------------------------------------------------------

#include <TUN/TUNhash.h>
#include <TUN/TUNkeyConvert.h>

#include <SFI/SFIbase64EncodeFilter.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring TUNhashKey(const COLstring& Der){
   COL_FUNCTION(TUNhashKey);
   return SFIbase64Encode(Der);
}

COLstring TUNhashKey(const TUNkey& Key){
   COL_FUNCTION(TUNhashKey);
   COLstring Der = TUNkeyPublicAsDer(Key);
   return TUNhashKey(Der);
}