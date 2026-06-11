// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNerror
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 13 June 2023 - 10:37AM
// ---------------------------------------------------------------------------

#include <TUN/TUNerror.h>

#include <openssl/err.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNthrowError(){
   unsigned long err_code = ERR_get_error();
   char err_msg[120];
   ERR_error_string_n(err_code, err_msg, sizeof(err_msg));
   throw COLerror(err_msg, err_code);
}