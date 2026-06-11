#ifndef __WEB_UTILS_H__
#define __WEB_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEButils
//
// Author: Michael Haddad
//
// Description:
//
// WEButils library
//---------------------------------------------------------------------------

class COLstring;

COLstring WEBlookupReason(int Code);
bool WEBisRedirectCode(int Code);

unsigned int WEBconvertHexString(const char* pValue, int Length);

#endif // end of defensive include
