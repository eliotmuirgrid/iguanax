#ifndef __TEST_CREDENTIALS_H__
#define __TEST_CREDENTIALS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCredentials
//
// Description:
//
// testCredentials - helper functions for credentials used in unit tests
//
// Author: Nicolas Lehman
// Date:   Thu 22 Apr 2021
//---------------------------------------------------------------------------

class COLvar;
class COLstring;

void GetCredentials(const COLstring& DbServerType, COLvar* pCredentials, bool UseLocal=true);

#endif // end of defensive include