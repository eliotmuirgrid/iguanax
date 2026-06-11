#ifndef __SDM_PERMISSIONS_H__
#define __SDM_PERMISSIONS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMpermissions
//
// Description:
//
// Function to give a user permission to login as a service.  So much code
// for such a little thing.
//
// Author:  Eliot Muir
// Date:    Thu 1/10/2022
//---------------------------------------------------------------------------

class COLstring;
class COLostream;

// TODO get rid of the COLostream& Log one.
bool SDMgiveUserRightToRunAsService(const COLstring& UserName, COLostream& Log);
bool SDMgiveUserRightToRunAsService(const COLstring& UserName, COLstring* pError);

bool SDMverifyPassword(const COLstring& Username, const COLstring& Password);

#endif // end of defensive include
