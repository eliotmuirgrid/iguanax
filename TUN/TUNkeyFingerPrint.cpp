// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNkeyFingerPrint
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Saturday 24 June 2023 - 10:51AM
// ---------------------------------------------------------------------------

#include <TUN/TUNkeyFingerPrint.h>
#include <TUN/TUNkeyConvert.h>

#include <SFI/SFImd5.h>
#include <SFI/SFIbase64EncodeFilter.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


COLstring TUNkeyFingerPrintFromDer(const COLstring& Der){
   COL_FUNCTION(TUNkeyFingerPrintFromDer);
   COLstring Md5 = SFImd5(Der);
   COLstring Result = SFIbase64Encode(Md5);
   return Result;
}


COLstring TUNkeyFingerPrint(const TUNkey& Key){
   COL_FUNCTION(TUNkeyFingerPrint);
   COLstring Der = TUNkeyPublicAsDer(Key);
   return TUNkeyFingerPrintFromDer(Der);
}   
