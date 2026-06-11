#ifndef __DB_DRIVER_VALIDATE_H__
#define __DB_DRIVER_VALIDATE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdriverValidate
//
// Description:
//
// Routines that validate if DLL/SO drivers are valid drivers
//
// Author: Eliot Muir
// Date:   Thu 15 Apr 2021 14:51:07 EDT
//---------------------------------------------------------------------------

#include <COL/COLostream.h>
#include <COL/COLdll.h>
class COLstring;

bool DBlibValidateAndLoad     (COLdll* MySqlDll, const COLstring& FileName);

#endif // end of defensive include
