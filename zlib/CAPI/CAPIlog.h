#ifndef __CAPI_LOG_H__
#define __CAPI_LOG_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CAPIlog
//
// Description:
//
// Wrapper API for stdout and stderr so we can unit test easily.
//
// Author: John Q
// Date:   Mon  4 Apr 2022 11:30:30 EDT
//---------------------------------------------------------------------------

class COLstring;
typedef void (*CAPIlogFcn)(const COLstring&);

void CAPIlogStdout(const COLstring& Message);
void CAPIlogStderr(const COLstring& Message);

void CAPIredirectStdout(CAPIlogFcn pCallback);
void CAPIredirectStderr(CAPIlogFcn pCallback);

#endif // end of defensive include
