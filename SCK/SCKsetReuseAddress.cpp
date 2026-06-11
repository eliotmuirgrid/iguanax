//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKsetReuseAddress
//
// Author: Eliot Muir
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#ifdef _WIN32
#include <COL/COLwindows.h>
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifdef _WIN32
void SCKsetSocketOption(int Socket, int OptionName, BOOL Value){
   COL_FUNCTION(SCKsetSocketOption);
   int Result = 0;
   Result = ::setsockopt(Socket, SOL_SOCKET, OptionName, (char *)&Value, sizeof(Value));
   if (Result == -1){
      COLstring Error = COLlastErrorAsString();
      COL_ERR(Error);
   }
}
#endif // _WIN32

// Old comment from previous staff - Eliot - not 100% correct
// NEVER use SO_REUSEADDR on Windows. It is not remotely the same as POSIX.
// http://msdn.microsoft.com/en-us/library/ms740621(VS.85).aspx
void SCKsetReuseAddress(int Socket){
   COL_FUNCTION(SCKsetReuseAddress);
#ifndef _WIN32
   int Value = 1;
   int Result = ::setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, &Value, sizeof(Value));
   COL_VAR(Result);
#else
   //SCKsetSocketOption(Socket, SO_EXCLUSIVEADDRUSE, TRUE);
   //SCKsetSocketOption(Socket, SO_REUSEADDR,        TRUE);
#endif
}
