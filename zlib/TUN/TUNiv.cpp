// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNiv
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 13 June 2023 - 03:11PM
// ---------------------------------------------------------------------------

#include <TUN/TUNiv.h>
#include <TUN/TUNerror.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <openssl/rand.h>

void TUNiv(COLstring* pIV, int Size){
   COL_FUNCTION(TUNiv);
   if (pIV->capacity() <= Size){
      pIV->setCapacity(Size);
   }
   pIV->setSize(Size);
   if (!RAND_bytes((unsigned char*)pIV->data(), Size)){
      TUNthrowError();
   }
}